// src/main.cpp
//
// Entry point. Supports:
//   - Random city generation (default)
//   - CSV file loading (--csv)
//   - TSPLIB file loading (--tsplib)
//   - Multiple independent runs (--runs)
//   - Convergence CSV export (--output-csv)
//   - Crossover/mutation operator selection (--crossover, --mutation)

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

#include "include/cli/options.h"
#include "include/ga/population.h"
#include "include/tsp/city.h"
#include "include/tsp/tsplib_parser.h"
#include "include/visualization/renderer.h"

namespace {

struct RunResult {
    double best_distance = 0.0;
    int best_generation = 0;
    std::vector<int> best_route;
};

static void print_final_results(const std::vector<tsp::City>& cities,
                                const std::vector<int>& best_route,
                                int n) {
    std::printf("\n===== Final Results =====\n");
    std::printf("Cities:        %d\n", n);

    double best_distance = 0.0;
    for (int i = 0; i < n - 1; ++i) {
        best_distance += tsp::distance(&cities[best_route[i]], &cities[best_route[i + 1]]);
    }
    best_distance += tsp::distance(&cities[best_route[n - 1]], &cities[best_route[0]]);
    std::printf("Best distance: %.2f\n\n", best_distance);

    std::printf("Best route (visiting order, returns to the start at the end):\n");
    for (int i = 0; i < n; ++i) {
        const auto& city = cities[best_route[i]];
        std::printf("    %2d: city %d   (%6.2f, %6.2f)\n", i, best_route[i], city.x, city.y);
    }
    const auto& first = cities[best_route[0]];
    std::printf("    %2d: city %d   (%6.2f, %6.2f)  <- back to start\n",
                n, best_route[0], first.x, first.y);
}

static RunResult run_ga(const std::vector<tsp::City>& cities,
                        const tsp::cli::Config& config,
                        std::mt19937& rng,
                        std::ofstream* csv_out) {
    const int n = static_cast<int>(cities.size());

    tsp::ga::Population pop =
        tsp::ga::init_population(cities.data(), n, config.population_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    int generation = 0;
    const int max_generations = config.generations;

    double global_best = std::numeric_limits<double>::infinity();
    std::vector<int> global_best_route;
    int global_best_gen = 0;

    while (generation < max_generations) {
        tsp::ga::next_generation(pop, cities.data(), n,
                                 config.mutation_rate, rng,
                                 config.crossover, config.mutation);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);

        // Find best route this generation
        int best_idx = 0;
        double gen_avg = 0.0;
        for (int i = 0; i < config.population_size; ++i) {
            gen_avg += pop.routes[i].fitness;
            if (pop.routes[i].fitness < pop.routes[best_idx].fitness) {
                best_idx = i;
            }
        }
        gen_avg /= config.population_size;

        if (pop.routes[best_idx].fitness < global_best) {
            global_best = pop.routes[best_idx].fitness;
            global_best_route = pop.routes[best_idx].route;
            global_best_gen = generation;
        }

        // Write CSV row if requested
        if (csv_out && csv_out->is_open()) {
            *csv_out << generation << "," << global_best << "," << gen_avg << "\n";
        }

        generation++;
    }

    // NOTE: no post-loop "final best" rescan here (a previous version had
    // one). It's dead code by construction: the while loop above already
    // evaluates fitness and checks every route against global_best on its
    // very last iteration, including the final generation. Re-scanning
    // `pop` again afterwards inspects the exact same, already-considered
    // data -- it can never find something better than what the loop just
    // recorded, so it was removed rather than kept as defensive-looking
    // code that doesn't actually defend against anything.

    RunResult result;
    result.best_distance = global_best;
    result.best_generation = global_best_gen;
    result.best_route = global_best_route;
    return result;
}

} // namespace

int main(int argc, char** argv) {
    auto parse_result = tsp::cli::parse_args(argc, argv);
    if (parse_result.status != tsp::cli::ParseStatus::Success) {
        return (parse_result.status == tsp::cli::ParseStatus::ExitOk) ? 0 : 1;
    }

    const auto& config = parse_result.config;

    // -----------------------------------------------------------------------
    // Load cities: TSPLIB > CSV > random
    // -----------------------------------------------------------------------
    std::vector<tsp::City> cities;

    if (!config.tsplib_file.empty()) {
        if (!tsp::load_tsplib(config.tsplib_file, cities)) {
            return 1;  // error already printed
        }
        std::printf("Loaded %zu cities from TSPLIB file: %s\n", cities.size(), config.tsplib_file.c_str());
    } else if (!config.csv_file.empty()) {
        if (!tsp::load_cities_from_csv(config.csv_file, cities)) {
            return 1;  // error already printed
        }
        std::printf("Loaded %zu cities from CSV file: %s\n", cities.size(), config.csv_file.c_str());
    } else {
        cities.resize(config.num_cities);
        std::mt19937 seed_rng;
        if (config.seed != 0) {
            seed_rng.seed(config.seed);
        } else {
            std::random_device rd;
            seed_rng.seed(rd());
        }
        tsp::init_cities_random(cities.data(), config.num_cities, seed_rng);
    }

    const int n = static_cast<int>(cities.size());

    // -----------------------------------------------------------------------
    // Open CSV output if requested
    // -----------------------------------------------------------------------
    std::ofstream csv_file;
    if (!config.output_csv.empty()) {
        csv_file.open(config.output_csv);
        if (!csv_file.is_open()) {
            std::fprintf(stderr, "Error: could not open CSV output file: %s\n",
                         config.output_csv.c_str());
            return 1;
        }
        csv_file << "generation,best_fitness,avg_fitness\n";
    }

    // -----------------------------------------------------------------------
    // Run GA: single or multiple runs
    // -----------------------------------------------------------------------
    std::vector<double> run_distances;
    RunResult best_overall;
    best_overall.best_distance = std::numeric_limits<double>::infinity();

    for (int run = 0; run < config.runs; ++run) {
        std::mt19937 rng;
        if (config.seed != 0) {
            // For multiple runs with fixed seed, vary per run for diversity
            rng.seed(config.seed + static_cast<unsigned int>(run));
        } else {
            std::random_device rd;
            rng.seed(rd());
        }

        std::printf("\n--- Run %d/%d ---\n", run + 1, config.runs);

        std::ofstream* csv_ptr = (run == 0 && csv_file.is_open()) ? &csv_file : nullptr;
        auto result = run_ga(cities, config, rng, csv_ptr);

        run_distances.push_back(result.best_distance);

        if (result.best_distance < best_overall.best_distance) {
            best_overall = result;
        }

        std::printf("Best distance: %.2f (found at generation %d)\n",
                    result.best_distance, result.best_generation);
    }

    if (csv_file.is_open()) {
        csv_file.close();
        std::printf("\nCSV output written to: %s\n", config.output_csv.c_str());
    }

    // -----------------------------------------------------------------------
    // Print aggregate statistics for multi-run
    // -----------------------------------------------------------------------
    if (config.runs > 1) {
        double sum = 0.0;
        double min_dist = run_distances[0];
        double max_dist = run_distances[0];
        for (double d : run_distances) {
            sum += d;
            if (d < min_dist) min_dist = d;
            if (d > max_dist) max_dist = d;
        }
        double avg = sum / run_distances.size();

        double variance = 0.0;
        for (double d : run_distances) {
            variance += (d - avg) * (d - avg);
        }
        double stddev = std::sqrt(variance / run_distances.size());

        std::printf("\n===== Aggregate Statistics (%d runs) =====\n", config.runs);
        std::printf("Best:  %.2f\n", min_dist);
        std::printf("Worst: %.2f\n", max_dist);
        std::printf("Avg:   %.2f\n", avg);
        std::printf("Std:   %.2f\n", stddev);
    }

    // -----------------------------------------------------------------------
    // Visualization (best overall run, only if not headless)
    // -----------------------------------------------------------------------
    // Note: we only store the single best route found across all runs, not
    // a per-generation history, so this shows the final result rather than
    // a live replay of the evolution. The single loop below both displays
    // it and waits for the user to close the window -- the previous version
    // had two separate loops here (a fixed-300-frame "replay" of a static
    // image, then a second blocking wait), which was redundant and, in the
    // second loop's case, never actually pumped window events (see the fix
    // in wait_for_close()/should_close() in renderer.cpp) -- so the window
    // could hang indefinitely and get flagged "Not Responding" by the OS.
    if (!config.headless) {
        tsp::visualization::init_window(n);

        while (!tsp::visualization::should_close()) {
            tsp::visualization::draw_route(cities.data(), n,
                                           best_overall.best_route.data(),
                                           best_overall.best_generation, config.generations);
        }

        tsp::visualization::close_window();
    }

    // Final results
    print_final_results(cities, best_overall.best_route, n);

    return 0;
}
