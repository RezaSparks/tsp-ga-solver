// include/ga/population.h
//
// The genetic algorithm core: a Population of Route candidates, plus the
// operators (selection, crossover, mutation, elitism) used to evolve one
// generation into the next.
//
// This is a direct port of the original population.cpp/.h logic. The
// malloc/free-based memory management from the original C-style code is
// replaced with std::vector/RAII so the code is memory-safe and exception
// safe by construction -- this is the one deliberate improvement made
// during the C++ port, since "manual malloc of a struct full of raw int*"
// is not something we want to carry forward into a C++17 codebase.
#pragma once

#include <vector>

#include "include/tsp/city.h"

namespace tsp::ga {

    // A single candidate solution: a permutation of city indices, plus its
    // cached fitness (total tour length -- lower is better).
    struct Route {
        std::vector<int> route;
        double fitness = 0.0;
    };

    // A generation of candidate routes.
    struct Population {
        std::vector<Route> routes;
    };

    // Builds an initial population of `pop_size` random permutations of the
    // `n` cities in `cities`.
    Population init_population(const tsp::City* cities, int n, int pop_size);

    // Computes and stores the tour length of every route in `pop`.
    void evaluate_fitness(Population& pop, const tsp::City* cities, int n);

    // Tournament selection (tournament size 3): picks two parent indices from
    // `pop` by repeatedly sampling 3 candidates and keeping the fittest.
    void tournament_selection(const Population& pop, int* parent1, int* parent2);

    // Ordered-crossover-style operator: copies a random slice of `p1` into
    // `child`, then fills remaining positions with the unused cities from `p2`
    // in order. Produces a valid permutation (no duplicate/missing cities).
    void crossover(const std::vector<int>& p1, const std::vector<int>& p2,
        std::vector<int>& child, int n);

    // Swap mutation: picks two random positions in `route` and swaps them.
    void mutate(std::vector<int>& route, int n);

    // Advances `pop` by one generation in place: keeps the best 2 routes
    // (elitism), then fills the rest via tournament selection + crossover +
    // mutation (applied with probability `pm`).
    void next_generation(Population& pop, const tsp::City* cities, int n, double pm);

} // namespace tsp::ga
