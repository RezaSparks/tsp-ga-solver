// src/main.cpp
#include <iostream>
#include <memory>
#include <random>

#include "include/cli/options.h"
#include "include/ga/population.h"
#include "include/tsp/city.h"
#include "include/visualization/renderer.h"

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
        std::printf("    %2d: city %d   (%3d, %3d)\n", i, best_route[i], city.x, city.y);
    }
    // closing edge: back to start
    const auto& first = cities[best_route[0]];
    std::printf("    %2d: city %d   (%3d, %3d)  <- back to start\n", n, best_route[0], first.x, first.y);
}

int main(int argc, char** argv) {
    auto parse_result = tsp::cli::parse_args(argc, argv);
    if (parse_result.status != tsp::cli::ParseStatus::Success) {
        return (parse_result.status == tsp::cli::ParseStatus::ExitOk) ? 0 : 1;
    }

    const auto& config = parse_result.config;

    // Modern C++ random number generation
    std::mt19937 rng;
    if (config.seed != 0) {
        rng.seed(config.seed);
    } else {
        std::random_device rd;
        rng.seed(rd());
    }

    // Generate random cities
    std::vector<tsp::City> cities(config.num_cities);
    tsp::init_cities_random(cities.data(), config.num_cities, rng);

    // Initialize and evaluate population
    tsp::ga::Population pop =
        tsp::ga::init_population(cities.data(), config.num_cities,
                                 config.population_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), config.num_cities);

    int generation = 0;
    const int max_generations = config.generations;

    // Raylib visualization (only if not headless)
    if (!config.headless) {
        tsp::visualization::init_window(config.num_cities);
    }

    while (generation < max_generations) {
        tsp::ga::next_generation(pop, cities.data(), config.num_cities,
                                 config.mutation_rate, rng);
        tsp::ga::evaluate_fitness(pop, cities.data(), config.num_cities);

        // Find best route
        int best_idx = 0;
        for (int i = 1; i < config.population_size; ++i) {
            if (pop.routes[i].fitness < pop.routes[best_idx].fitness) {
                best_idx = i;
            }
        }

        if (!config.headless) {
            tsp::visualization::draw_route(cities.data(), config.num_cities,
                                           pop.routes[best_idx].route.data(),
                                           generation, max_generations);
        }

        generation++;
    }

    // Find final best route
    int best_idx = 0;
    for (int i = 1; i < config.population_size; ++i) {
        if (pop.routes[i].fitness < pop.routes[best_idx].fitness) {
            best_idx = i;
        }
    }

    if (!config.headless) {
        // Pause before closing so user can see final state
        tsp::visualization::wait_for_close();
        tsp::visualization::close_window();
    }

    // Print final results (works in both headless and windowed modes)
    print_final_results(cities, pop.routes[best_idx].route, config.num_cities);

    return 0;
}