#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <chrono>
#include <iomanip>
#include <filesystem>

#include "include/cli/options.h"
#include "include/ga/population.h"
#include "include/tsp/city.h"
#include "include/tsp/tsplib_parser.h"

namespace fs = std::filesystem;

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

static RunResult run_ga(const std::vector<tsp::City>& cities,
                        const tsp::cli::Config& config,
                        std::mt19937& rng,
                        std::vector<std::pair<double, double>>& history) {
    const int n = static_cast<int>(cities.size());

    tsp::ga::Population pop =
        tsp::ga::init_population(cities.data(), n, config.population_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    double global_best = std::numeric_limits<double>::infinity();
    std::vector<int> global_best_route;
    int global_best_gen = 0;

    history.clear();
    history.reserve(config.generations);

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

        history.push_back({global_best, gen_avg});
    }

    RunResult result;
    result.best_distance = global_best;
    result.best_generation = global_best_gen;
    result.best_route = global_best_route;
    return result;
}

static void save_best_route_csv(const std::vector<tsp::City>& cities,
                                const std::vector<int>& route,
                                const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::fprintf(stderr, "Warning: could not open %s for writing\n", filename.c_str());
        return;
    }
    file << "index,x,y\n";   // ← ستون index اضافه شد
    for (int idx : route) {
        file << idx << "," << cities[idx].x << "," << cities[idx].y << "\n";
    }
    file.close();
    std::printf("Best route coordinates written to: %s\n", filename.c_str());
}

static void save_convergence_csv(const std::vector<std::pair<double, double>>& history,
                                 const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::fprintf(stderr, "Warning: could not open %s for writing\n", filename.c_str());
        return;
    }
    file << "generation,best_fitness,avg_fitness\n";
    for (size_t i = 0; i < history.size(); ++i) {
        file << i << "," << history[i].first << "," << history[i].second << "\n";
    }
    file.close();
    std::printf("Convergence data written to: %s\n", filename.c_str());
}

static void save_params_file(const tsp::cli::Config& config,
                             int num_cities,
                             double best_distance,
                             const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::fprintf(stderr, "Warning: could not open %s for writing\n", filename.c_str());
        return;
    }
    file << "Cities: " << num_cities << "\n";
    file << "Population: " << config.population_size << "\n";
    file << "Generations: " << config.generations << "\n";
    file << "Mutation Rate: " << config.mutation_rate << "\n";
    file << "Crossover: " << static_cast<int>(config.crossover) << "\n";
    file << "Mutation: " << static_cast<int>(config.mutation) << "\n";
    file << "Seed: " << config.seed << "\n";
    file << "Best Distance: " << std::fixed << std::setprecision(2) << best_distance << "\n";
    file.close();
}

} // namespace

int main(int argc, char** argv) {
    auto parse_result = tsp::cli::parse_args(argc, argv);
    if (parse_result.status != tsp::cli::ParseStatus::Success) {
        return (parse_result.status == tsp::cli::ParseStatus::ExitOk) ? 0 : 1;
    }
    const auto& config = parse_result.config;

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

    std::string timestamp = std::to_string(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );

    std::string output_dir = "output_" + timestamp;
    fs::create_directory(output_dir);

    std::vector<double> run_distances;
    RunResult best_overall;
    best_overall.best_distance = std::numeric_limits<double>::infinity();

    std::ofstream csv_file;
    if (!config.output_csv.empty()) {
        csv_file.open(config.output_csv);
        if (!csv_file.is_open()) {
            std::fprintf(stderr, "Error: could not open CSV output: %s\n", config.output_csv.c_str());
            return 1;
        }
        csv_file << "generation,best_fitness,avg_fitness\n";
    }

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

        std::vector<std::pair<double, double>> history;
        RunResult result = run_ga(cities, config, rng, history);

        run_distances.push_back(result.best_distance);

        if (result.best_distance < best_overall.best_distance) {
            best_overall = result;
        }

        std::printf("Best distance: %.2f (found at generation %d)\n",
                    result.best_distance, result.best_generation);

        if (run == 0) {
            save_convergence_csv(history, output_dir + "/convergence.csv");
            if (csv_file.is_open()) {
                for (size_t i = 0; i < history.size(); ++i) {
                    csv_file << i << "," << history[i].first << "," << history[i].second << "\n";
                }
            }
        }
    }

    if (csv_file.is_open()) {
        csv_file.close();
        std::printf("\nConvergence data written to: %s\n", config.output_csv.c_str());
    }

    if (!best_overall.best_route.empty()) {
        save_best_route_csv(cities, best_overall.best_route, output_dir + "/best_route.csv");
        save_params_file(config, n, best_overall.best_distance, output_dir + "/params.txt");
    }

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

    print_final_results(cities, best_overall.best_route, n);

    std::printf("\nAll outputs saved to: %s/\n", output_dir.c_str());

    return 0;
}