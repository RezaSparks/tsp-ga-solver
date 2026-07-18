// src/population.cpp
#include "include/ga/population.h"

#include <algorithm>
#include <numeric>

namespace tsp::ga {

    Population init_population(const tsp::City* /*cities*/, int n, int pop_size,
                                 std::mt19937& rng) {
        Population pop;
        pop.routes.resize(pop_size);

        for (auto& r : pop.routes) {
            r.route.resize(n);
            std::iota(r.route.begin(), r.route.end(), 0);
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

    // -----------------------------------------------------------------------
    // Ordered Crossover (OX) — original, always valid
    // -----------------------------------------------------------------------
    static void crossover_ox(const std::vector<int>& p1, const std::vector<int>& p2,
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

    // -----------------------------------------------------------------------
    // Partially Mapped Crossover (PMX)
    // -----------------------------------------------------------------------
    // BUGFIX: the previous version mutated `val` while walking the conflict
    // chain, which turns the textbook (guaranteed-to-terminate) PMX
    // resolution step into one that can spin forever re-chasing values that
    // are already placed. This version keeps `val` fixed and walks a
    // position pointer until it lands outside the copied [start, end]
    // segment -- which is guaranteed to terminate, since p1/p2 are both
    // permutations. Also drops the previous std::unordered_map in favor of
    // a plain std::vector for the p2 position lookup: city indices are a
    // dense 0..n-1 range, so there's no reason to pay for hashing when a
    // direct array index does the same job faster and without per-call
    // heap allocations (this matches what crossover_cycle already does).
    static void crossover_pmx(const std::vector<int>& p1, const std::vector<int>& p2,
                              std::vector<int>& child, int n, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(0, n - 1);
        int start = dist(rng);
        int end = dist(rng);
        if (start > end) std::swap(start, end);

        child.assign(n, -1);

        // Marks which city VALUES have already been placed via p1's segment.
        std::vector<char> in_segment(n, 0);
        for (int i = start; i <= end; i++) {
            child[i] = p1[i];
            in_segment[p1[i]] = 1;
        }

        // pos_in_p2[v] = the index at which value v appears in p2.
        std::vector<int> pos_in_p2(n);
        for (int i = 0; i < n; i++) {
            pos_in_p2[p2[i]] = i;
        }

        // For every position in the segment, if p2's value there wasn't
        // already brought in by p1's segment, it needs a home outside the
        // segment. Walk the p1/p2 mapping chain -- val stays fixed, only
        // the candidate position moves -- until we land outside [start,end].
        for (int i = start; i <= end; i++) {
            const int val = p2[i];
            if (in_segment[val]) continue;

            int pos = i;
            do {
                pos = pos_in_p2[p1[pos]];
            } while (pos >= start && pos <= end);

            child[pos] = val;
        }

        // Everything else is identical between p1 and p2's remaining slots,
        // so just copy directly from p2.
        for (int i = 0; i < n; i++) {
            if (child[i] == -1) child[i] = p2[i];
        }
    }

    // -----------------------------------------------------------------------
    // Cycle Crossover (CX)
    // -----------------------------------------------------------------------
    static void crossover_cycle(const std::vector<int>& p1, const std::vector<int>& p2,
                                std::vector<int>& child, int n, std::mt19937& /*rng*/) {
        child.assign(n, -1);
        std::vector<char> taken(n, 0);

        // Build position lookup for p2
        std::vector<int> pos_in_p2(n);
        for (int i = 0; i < n; i++) {
            pos_in_p2[p2[i]] = i;
        }

        bool use_p1 = true;
        for (int start = 0; start < n; start++) {
            if (child[start] != -1) continue;

            int current = start;
            do {
                if (use_p1) {
                    child[current] = p1[current];
                } else {
                    child[current] = p2[current];
                }
                taken[child[current]] = 1;
                current = pos_in_p2[p1[current]];
            } while (current != start && child[current] == -1);

            use_p1 = !use_p1;
        }
    }

    void crossover(const std::vector<int>& p1, const std::vector<int>& p2,
                   std::vector<int>& child, int n, std::mt19937& rng,
                   tsp::cli::CrossoverType type) {
        switch (type) {
            case tsp::cli::CrossoverType::OX:
                crossover_ox(p1, p2, child, n, rng);
                break;
            case tsp::cli::CrossoverType::PMX:
                crossover_pmx(p1, p2, child, n, rng);
                break;
            case tsp::cli::CrossoverType::Cycle:
                crossover_cycle(p1, p2, child, n, rng);
                break;
        }
    }

    // -----------------------------------------------------------------------
    // Swap mutation — original
    // -----------------------------------------------------------------------
    static void mutate_swap(std::vector<int>& route, int n, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(0, n - 1);
        int i = dist(rng);
        int j = dist(rng);
        std::swap(route[i], route[j]);
    }

    // -----------------------------------------------------------------------
    // Inversion mutation — reverse a random sub-segment
    // -----------------------------------------------------------------------
    static void mutate_inversion(std::vector<int>& route, int n, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(0, n - 1);
        int i = dist(rng);
        int j = dist(rng);
        if (i > j) std::swap(i, j);
        std::reverse(route.begin() + i, route.begin() + j + 1);
    }

    // -----------------------------------------------------------------------
    // Scramble mutation — shuffle a random sub-segment
    // -----------------------------------------------------------------------
    static void mutate_scramble(std::vector<int>& route, int n, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(0, n - 1);
        int i = dist(rng);
        int j = dist(rng);
        if (i > j) std::swap(i, j);
        std::shuffle(route.begin() + i, route.begin() + j + 1, rng);
    }

    void mutate(std::vector<int>& route, int n, std::mt19937& rng,
                tsp::cli::MutationType type) {
        switch (type) {
            case tsp::cli::MutationType::Swap:
                mutate_swap(route, n, rng);
                break;
            case tsp::cli::MutationType::Inversion:
                mutate_inversion(route, n, rng);
                break;
            case tsp::cli::MutationType::Scramble:
                mutate_scramble(route, n, rng);
                break;
        }
    }

    void next_generation(Population& pop, const tsp::City* cities, int n,
                         double pm, std::mt19937& rng,
                         tsp::cli::CrossoverType cx_type,
                         tsp::cli::MutationType mut_type) {
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
                      new_pop.routes[i].route, n, rng, cx_type);
            if (prob_dist(rng) < pm) {
                mutate(new_pop.routes[i].route, n, rng, mut_type);
            }
        }

        pop.routes = std::move(new_pop.routes);
        (void)cities; // fitness not recomputed here, same as original
    }

} // namespace tsp::ga
