// tests/test_core.cpp — Issue #4 + #5
// Tests: distance correctness, crossover validity, elitism property

#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <numeric>
#include <limits>

#include "include/tsp/city.h"
#include "include/ga/population.h"

namespace {

bool is_valid_permutation(const std::vector<int>& route, int n) {
    if (static_cast<int>(route.size()) != n) return false;
    std::vector<char> seen(n, 0);
    for (int city : route) {
        if (city < 0 || city >= n) return false;
        if (seen[city]) return false;
        seen[city] = 1;
    }
    return true;
}

// === Test 1: Distance ===
TEST(DistanceTest, SameCityZeroDistance) {
    tsp::City a{10, 20};
    tsp::City b{10, 20};
    EXPECT_DOUBLE_EQ(tsp::distance(&a, &b), 0.0);
}

TEST(DistanceTest, KnownDistance) {
    tsp::City a{0, 0};
    tsp::City b{3, 4};
    EXPECT_DOUBLE_EQ(tsp::distance(&a, &b), 5.0);
}

TEST(DistanceTest, Symmetry) {
    tsp::City a{100, 200};
    tsp::City b{300, 500};
    EXPECT_DOUBLE_EQ(tsp::distance(&a, &b), tsp::distance(&b, &a));
}

// === Test 2: Crossover Validity ===
TEST(CrossoverValidity, OrderedCrossoverIsValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> p1(n), p2(n), child(n);
    std::iota(p1.begin(), p1.end(), 0);
    std::iota(p2.begin(), p2.end(), 0);
    std::shuffle(p1.begin(), p1.end(), rng);
    std::shuffle(p2.begin(), p2.end(), rng);

    std::mt19937 crossover_rng(123);
    tsp::ga::crossover(p1, p2, child, n, crossover_rng);

    EXPECT_TRUE(is_valid_permutation(child, n));
}

TEST(CrossoverValidity, CrossoverWithDifferentSeeds) {
    const int n = 50;
    for (int seed = 1; seed <= 20; ++seed) {
        std::mt19937 rng1(seed);
        std::mt19937 rng2(seed + 1000);
        std::vector<int> p1(n), p2(n), child(n);
        std::iota(p1.begin(), p1.end(), 0);
        std::iota(p2.begin(), p2.end(), 0);
        std::shuffle(p1.begin(), p1.end(), rng1);
        std::shuffle(p2.begin(), p2.end(), rng2);

        std::mt19937 crossover_rng(seed + 500);
        tsp::ga::crossover(p1, p2, child, n, crossover_rng);
        EXPECT_TRUE(is_valid_permutation(child, n))
            << "OX failed with seed " << seed;
    }
}

// === Test 3: Elitism Property ===
TEST(ElitismProperty, BestFitnessNeverWorsensOver50Generations) {
    std::mt19937 rng(42);
    const int n = 15, pop_size = 50, generations = 50;
    const double mutation_rate = 0.02;

    std::vector<tsp::City> cities(n);
    tsp::init_cities_random(cities.data(), n, rng);

    auto pop = tsp::ga::init_population(cities.data(), n, pop_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    double prev_best = std::numeric_limits<double>::infinity();
    for (const auto& r : pop.routes) {
        if (r.fitness < prev_best) prev_best = r.fitness;
    }

    for (int gen = 0; gen < generations; ++gen) {
        tsp::ga::next_generation(pop, cities.data(), n, mutation_rate, rng);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);

        double curr_best = std::numeric_limits<double>::infinity();
        for (const auto& r : pop.routes) {
            if (r.fitness < curr_best) curr_best = r.fitness;
        }

        EXPECT_LE(curr_best, prev_best + 1e-9)
            << "Elitism failed at generation " << gen
            << ": " << prev_best << " -> " << curr_best;

        prev_best = curr_best;
    }
}

TEST(ElitismProperty, BestFitnessNeverWorsensWithHighMutation) {
    std::mt19937 rng(99);
    const int n = 10, pop_size = 30, generations = 30;
    const double mutation_rate = 0.5;

    std::vector<tsp::City> cities(n);
    tsp::init_cities_random(cities.data(), n, rng);

    auto pop = tsp::ga::init_population(cities.data(), n, pop_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    double prev_best = std::numeric_limits<double>::infinity();
    for (const auto& r : pop.routes) {
        if (r.fitness < prev_best) prev_best = r.fitness;
    }

    for (int gen = 0; gen < generations; ++gen) {
        tsp::ga::next_generation(pop, cities.data(), n, mutation_rate, rng);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);

        double curr_best = std::numeric_limits<double>::infinity();
        for (const auto& r : pop.routes) {
            if (r.fitness < curr_best) curr_best = r.fitness;
        }

        EXPECT_LE(curr_best, prev_best + 1e-9)
            << "Elitism failed under high mutation at gen " << gen;

        prev_best = curr_best;
    }
}

} // namespace