// include/ga/population.h
//
// The genetic algorithm core: a Population of Route candidates, plus the
// operators (selection, crossover, mutation, elitism) used to evolve one
// generation into the next.
// Issue #10: crossover type dispatch
// Issue #11: mutation type dispatch
#pragma once

#include <random>
#include <vector>

#include "include/tsp/city.h"
#include "include/cli/options.h"

namespace tsp::ga {

    struct Route {
        std::vector<int> route;
        double fitness = 0.0;
    };

    struct Population {
        std::vector<Route> routes;
    };

    Population init_population(const tsp::City* cities, int n, int pop_size,
                               std::mt19937& rng);

    void evaluate_fitness(Population& pop, const tsp::City* cities, int n);

    void tournament_selection(const Population& pop, int* parent1, int* parent2,
                              std::mt19937& rng);

    // Issue #10: crossover dispatch
    void crossover(const std::vector<int>& p1, const std::vector<int>& p2,
                   std::vector<int>& child, int n, std::mt19937& rng,
                   tsp::cli::CrossoverType type);

    // Issue #11: mutation dispatch
    void mutate(std::vector<int>& route, int n, std::mt19937& rng,
                tsp::cli::MutationType type);

    // Issue #10+11: next_generation now accepts operator types
    void next_generation(Population& pop, const tsp::City* cities, int n,
                         double pm, std::mt19937& rng,
                         tsp::cli::CrossoverType cx_type,
                         tsp::cli::MutationType mut_type);

} // namespace tsp::ga
