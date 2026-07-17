// include/ga/population.h
#pragma once

#include <random>
#include <vector>

#include "include/tsp/city.h"

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

void crossover(const std::vector<int>& p1, const std::vector<int>& p2,
               std::vector<int>& child, int n, std::mt19937& rng);

void mutate(std::vector<int>& route, int n, std::mt19937& rng);

void next_generation(Population& pop, const tsp::City* cities, int n,
                     double pm, std::mt19937& rng);

} // namespace tsp::ga