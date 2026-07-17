// src/population.cpp
#include "include/ga/population.h"

#include <algorithm>
#include <numeric>

namespace tsp::ga {

Population init_population(const tsp::City* /*cities*/, int n, int pop_size,
                           std::mt19937& rng) {
    Population pop;
    pop.routes.resize(pop_size);

    std::uniform_int_distribution<int> dist(0, n - 1);

    for (auto& r : pop.routes) {
        r.route.resize(n);
        std::iota(r.route.begin(), r.route.end(), 0);
        // Fisher-Yates shuffle using modern RNG
        for (int j = n - 1; j > 0; j--) {
            std::uniform_int_distribution<int> idx_dist(0, j);
            int k = idx_dist(rng);
            std::swap(r.route[j], r.route[k]);
        }
        r.fitness = 0.0;
    }
    return pop;
}

void evaluate_fitness(Population& pop, const tsp::City* cities, int n) {
    for (auto& r : pop.routes) {
        double total = 0.0;
        for (int j = 0; j < n - 1; j++) {
            total += tsp::distance(&cities[r.route[j]], &cities[r.route[j + 1]]);
        }
        total += tsp::distance(&cities[r.route[n - 1]], &cities[r.route[0]]);
        r.fitness = total;
    }
}

void tournament_selection(const Population& pop, int* p1, int* p2,
                          std::mt19937& rng) {
    const int k = 3;
    const int pop_size = static_cast<int>(pop.routes.size());
    std::uniform_int_distribution<int> dist(0, pop_size - 1);

    int idx1 = dist(rng);
    for (int i = 1; i < k; i++) {
        int c = dist(rng);
        if (pop.routes[c].fitness < pop.routes[idx1].fitness) idx1 = c;
    }

    int idx2 = dist(rng);
    for (int i = 1; i < k; i++) {
        int c = dist(rng);
        if (pop.routes[c].fitness < pop.routes[idx2].fitness) idx2 = c;
    }

    *p1 = idx1;
    *p2 = idx2;
}

void crossover(const std::vector<int>& p1, const std::vector<int>& p2,
               std::vector<int>& child, int n, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, n - 1);
    int start = dist(rng);
    int end = dist(rng);
    if (start > end) std::swap(start, end);

    std::vector<char> used(n, 0);
    for (int i = start; i <= end; i++) {
        child[i] = p1[i];
        used[p1[i]] = 1;
    }

    int pos = 0;
    for (int i = 0; i < n; i++) {
        if (pos == start) pos = end + 1;
        if (pos >= n) break;
        if (!used[p2[i]]) {
            child[pos++] = p2[i];
            used[p2[i]] = 1;
        }
    }
}

void mutate(std::vector<int>& route, int n, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, n - 1);
    int i = dist(rng);
    int j = dist(rng);
    std::swap(route[i], route[j]);
}

void next_generation(Population& pop, const tsp::City* cities, int n,
                     double pm, std::mt19937& rng) {
    const int pop_size = static_cast<int>(pop.routes.size());

    Population new_pop;
    new_pop.routes.resize(pop_size);
    for (auto& r : new_pop.routes) r.route.resize(n);

    // Elitism: carry the two best routes over unchanged.
    int best = 0, second = 1;
    for (int i = 1; i < pop_size; i++) {
        if (pop.routes[i].fitness < pop.routes[best].fitness) {
            second = best;
            best = i;
        } else if (pop.routes[i].fitness < pop.routes[second].fitness) {
            second = i;
        }
    }
    new_pop.routes[0].route = pop.routes[best].route;
    new_pop.routes[1].route = pop.routes[second].route;

    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

    for (int i = 2; i < pop_size; i++) {
        int p1, p2;
        tournament_selection(pop, &p1, &p2, rng);
        crossover(pop.routes[p1].route, pop.routes[p2].route,
                  new_pop.routes[i].route, n, rng);
        if (prob_dist(rng) < pm) {
            mutate(new_pop.routes[i].route, n, rng);
        }
    }

    pop.routes = std::move(new_pop.routes);
    (void)cities; // fitness not recomputed here, same as original
}

} // namespace tsp::ga