// src/main.cpp
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <chrono>
#include <thread>

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

    std::printf("Best route (visiting order, returns to start):\n");
    for (int i = 0; i < n; ++i) {
        const auto& city = cities[best_route[i]];
        std::printf("    %2d: city %d   (%8.2f, %8.2f)\n", i, best_route[i], city.x, city.y);
    }
    const auto& first = cities[best_route[0]];
    std::printf("    %2d: city %d   (%8.2f, %8.2f)  <- back to start\n",
                n, best_route[0], first.x, first.y);
}

// Headless GA: no drawing
static RunResult run_ga_headless(const std::vector<tsp::City>& cities,
                                 const tsp::cli::Config& config,
                                 std::mt19937& rng,
                                 std::ofstream* csv_out) {
    const int n = static_cast<int>(cities.size());

    tsp::ga::Population pop =
        tsp::ga::init_population(cities.data(), n, config.population_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    double global_best = std::numeric_limits<double>::infinity();
    std::vector<int> global_best_route;
    int global_best_gen = 0;

    for (int gen = 0; gen < config.generations; ++gen) {
        tsp::ga::next_generation(pop, cities.data(), n,
                                 config.mutation_rate, rng,
                                 config.crossover, config.mutation);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);

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
            global_best_gen = gen;
        }

        if (csv_out && csv_out->is_open()) {
            *csv_out << gen << "," << global_best << "," << gen_avg << "\n";
        }
    }

    RunResult result;
    result.best_distance = global_best;
    result.best_generation = global_best_gen;
    result.best_route = global_best_route;
    return result;
}

// GUI GA: draws every generation
static RunResult run_ga_gui(const std::vector<tsp::City>& cities,
                            const tsp::cli::Config& config,
                            std::mt19937& rng,
                            std::ofstream* csv_out) {
    const int n = static_cast<int>(cities.size());

    printf("[GUI] Initializing population...\n");
    tsp::ga::Population pop =
        tsp::ga::init_population(cities.data(), n, config.population_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    double global_best = std::numeric_limits<double>::infinity();
    std::vector<int> global_best_route;
    int global_best_gen = 0;

    for (int gen = 0; gen < config.generations; ++gen) {
        // Check if window was closed by user
        if (tsp::visualization::should_close()) {
            printf("[GUI] Window closed by user at generation %d\n", gen);
            break;
        }

        tsp::ga::next_generation(pop, cities.data(), n,
                                 config.mutation_rate, rng,
                                 config.crossover, config.mutation);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);

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
            global_best_gen = gen;
        }

        if (csv_out && csv_out->is_open()) {
            *csv_out << gen << "," << global_best << "," << gen_avg << "\n";
        }

        // Draw current best (or current generation's best if none found yet)
        const int* route_to_draw = global_best_route.empty()
                                   ? pop.routes[best_idx].route.data()
                                   : global_best_route.data();
        tsp::visualization::draw_route(cities.data(), n,
                                       route_to_draw,
                                       gen, config.generations);
    }

    // ---- FINAL SOLUTION LOOP ----
    // Keep the window open even if should_close() is true, by forcing a 10-second delay.
    if (!global_best_route.empty()) {
        printf("\n[GUI] GA finished. Showing final solution for 10 seconds...\n");
        printf("[GUI] You can close the window manually to exit sooner.\n");

        int delay_seconds = 10;
        auto start_time = std::chrono::steady_clock::now();
        while (true) {
            // Draw the route
            tsp::visualization::draw_route(cities.data(), n,
                                           global_best_route.data(),
                                           global_best_gen, config.generations);

            // If the user closes the window, break early
            if (tsp::visualization::should_close()) {
                printf("[GUI] Window closed by user.\n");
                break;
            }

            // Check if 10 seconds have passed
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() >= delay_seconds) {
                printf("[GUI] 10 seconds elapsed. Closing window.\n");
                break;
            }

            // Yield to OS to keep the window responsive
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    } else {
        printf("[GUI] No solution found (global_best_route empty).\n");
    }

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
    // Load cities
    // -----------------------------------------------------------------------
    std::vector<tsp::City> cities;

    if (!config.tsplib_file.empty()) {
        if (!tsp::load_tsplib(config.tsplib_file, cities)) {
            return 1;
        }
        std::printf("Loaded %zu cities from TSPLIB: %s\n", cities.size(), config.tsplib_file.c_str());
    } else if (!config.csv_file.empty()) {
        if (!tsp::load_cities_from_csv(config.csv_file, cities)) {
            return 1;
        }
        std::printf("Loaded %zu cities from CSV: %s\n", cities.size(), config.csv_file.c_str());
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
        std::printf("Generated %d random cities\n", config.num_cities);
    }

    const int n = static_cast<int>(cities.size());

    // -----------------------------------------------------------------------
    // Open CSV output if requested
    // -----------------------------------------------------------------------
    std::ofstream csv_file;
    if (!config.output_csv.empty()) {
        csv_file.open(config.output_csv);
        if (!csv_file.is_open()) {
            std::fprintf(stderr, "Error: could not open CSV output: %s\n", config.output_csv.c_str());
            return 1;
        }
        csv_file << "generation,best_fitness,avg_fitness\n";
    }

    // -----------------------------------------------------------------------
    // Open window BEFORE starting GA (if GUI mode)
    // -----------------------------------------------------------------------
    if (!config.headless) {
        printf("[MAIN] Initializing window...\n");
        tsp::visualization::init_window(n);
        printf("[MAIN] Window initialized.\n");
    }

    // -----------------------------------------------------------------------
    // Run GA
    // -----------------------------------------------------------------------
    std::vector<double> run_distances;
    RunResult best_overall;
    best_overall.best_distance = std::numeric_limits<double>::infinity();

    for (int run = 0; run < config.runs; ++run) {
        std::mt19937 rng;
        if (config.seed != 0) {
            std::seed_seq seq{config.seed, static_cast<unsigned int>(run), 0x9E3779B9u};
            rng = std::mt19937(seq);
        } else {
            std::random_device rd;
            rng.seed(rd());
        }

        std::printf("\n--- Run %d/%d ---\n", run + 1, config.runs);

        std::ofstream* csv_ptr = (run == 0 && csv_file.is_open()) ? &csv_file : nullptr;

        RunResult result;
        if (config.headless) {
            result = run_ga_headless(cities, config, rng, csv_ptr);
        } else {
            result = run_ga_gui(cities, config, rng, csv_ptr);
        }

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
    // Print aggregate statistics
    // -----------------------------------------------------------------------
    if (config.runs > 1) {
        double sum = 0.0, min_dist = run_distances[0], max_dist = run_distances[0];
        for (double d : run_distances) {
            sum += d;
            if (d < min_dist) min_dist = d;
            if (d > max_dist) max_dist = d;
        }
        double avg = sum / run_distances.size();
        double variance = 0.0;
        for (double d : run_distances) variance += (d - avg) * (d - avg);
        double stddev = std::sqrt(variance / run_distances.size());

        std::printf("\n===== Aggregate Statistics (%d runs) =====\n", config.runs);
        std::printf("Best:  %.2f\n", min_dist);
        std::printf("Worst: %.2f\n", max_dist);
        std::printf("Avg:   %.2f\n", avg);
        std::printf("Std:   %.2f\n", stddev);
    }

    // -----------------------------------------------------------------------
    // Cleanup
    // -----------------------------------------------------------------------
    if (!config.headless) {
        tsp::visualization::close_window();
    }

    print_final_results(cities, best_overall.best_route, n);

    return 0;
}