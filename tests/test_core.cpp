// tests/test_core.cpp
// Issue #4 + #5 + #10 + #11: Core algorithm unit tests
//
// Tests:
//   1. DistanceTest       -- Euclidean distance correctness
//   2. CrossoverValidity  -- OX, PMX, Cycle produce valid permutations
//   3. MutationValidity   -- Swap, Inversion, Scramble produce valid permutations
//   4. ElitismProperty    -- best fitness never worsens

#include <gtest/gtest.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include "include/tsp/city.h"
#include "include/ga/population.h"
#include "include/cli/options.h"

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

// =========================================================================
// Distance Tests
// =========================================================================
TEST(DistanceTest, SameCityZeroDistance) {
    tsp::City a{10.0, 20.0};
    tsp::City b{10.0, 20.0};
    EXPECT_DOUBLE_EQ(tsp::distance(&a, &b), 0.0);
}

TEST(DistanceTest, KnownDistance) {
    tsp::City a{0.0, 0.0};
    tsp::City b{3.0, 4.0};
    EXPECT_DOUBLE_EQ(tsp::distance(&a, &b), 5.0);
}

TEST(DistanceTest, Symmetry) {
    tsp::City a{100.5, 200.3};
    tsp::City b{300.7, 500.1};
    EXPECT_DOUBLE_EQ(tsp::distance(&a, &b), tsp::distance(&b, &a));
}

TEST(DistanceTest, DoublePrecision) {
    tsp::City a{1.1, 2.2};
    tsp::City b{4.4, 6.6};
    double d = tsp::distance(&a, &b);
    EXPECT_GT(d, 0.0);
    EXPECT_TRUE(std::isfinite(d));
}

// =========================================================================
// Crossover Validity Tests (Issue #10)
// =========================================================================
TEST(CrossoverValidity, OXProducesValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> p1(n), p2(n), child(n);
    std::iota(p1.begin(), p1.end(), 0);
    std::iota(p2.begin(), p2.end(), 0);
    std::shuffle(p1.begin(), p1.end(), rng);
    std::shuffle(p2.begin(), p2.end(), rng);

    std::mt19937 cx_rng(123);
    tsp::ga::crossover(p1, p2, child, n, cx_rng, tsp::cli::CrossoverType::OX);
    EXPECT_TRUE(is_valid_permutation(child, n));
}

TEST(CrossoverValidity, PMXProducesValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> p1(n), p2(n), child(n);
    std::iota(p1.begin(), p1.end(), 0);
    std::iota(p2.begin(), p2.end(), 0);
    std::shuffle(p1.begin(), p1.end(), rng);
    std::shuffle(p2.begin(), p2.end(), rng);

    std::mt19937 cx_rng(456);
    tsp::ga::crossover(p1, p2, child, n, cx_rng, tsp::cli::CrossoverType::PMX);
    EXPECT_TRUE(is_valid_permutation(child, n));
}

TEST(CrossoverValidity, CycleProducesValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> p1(n), p2(n), child(n);
    std::iota(p1.begin(), p1.end(), 0);
    std::iota(p2.begin(), p2.end(), 0);
    std::shuffle(p1.begin(), p1.end(), rng);
    std::shuffle(p2.begin(), p2.end(), rng);

    std::mt19937 cx_rng(789);
    tsp::ga::crossover(p1, p2, child, n, cx_rng, tsp::cli::CrossoverType::Cycle);
    EXPECT_TRUE(is_valid_permutation(child, n));
}

TEST(CrossoverValidity, AllCrossoversValidWithManySeeds) {
    const int n = 30;
    for (int seed = 1; seed <= 10; ++seed) {
        std::mt19937 rng1(seed);
        std::mt19937 rng2(seed + 1000);
        std::vector<int> p1(n), p2(n), child(n);
        std::iota(p1.begin(), p1.end(), 0);
        std::iota(p2.begin(), p2.end(), 0);
        std::shuffle(p1.begin(), p1.end(), rng1);
        std::shuffle(p2.begin(), p2.end(), rng2);

        for (auto cx_type : {tsp::cli::CrossoverType::OX,
                              tsp::cli::CrossoverType::PMX,
                              tsp::cli::CrossoverType::Cycle}) {
            std::mt19937 cx_rng(seed + 500);
            tsp::ga::crossover(p1, p2, child, n, cx_rng, cx_type);
            EXPECT_TRUE(is_valid_permutation(child, n))
                << "Crossover failed with seed " << seed;
        }
    }
}

// =========================================================================
// Mutation Validity Tests (Issue #11)
// =========================================================================
TEST(MutationValidity, SwapProducesValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> route(n);
    std::iota(route.begin(), route.end(), 0);
    std::shuffle(route.begin(), route.end(), rng);

    std::mt19937 mut_rng(123);
    tsp::ga::mutate(route, n, mut_rng, tsp::cli::MutationType::Swap);
    EXPECT_TRUE(is_valid_permutation(route, n));
}

TEST(MutationValidity, InversionProducesValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> route(n);
    std::iota(route.begin(), route.end(), 0);
    std::shuffle(route.begin(), route.end(), rng);

    std::mt19937 mut_rng(456);
    tsp::ga::mutate(route, n, mut_rng, tsp::cli::MutationType::Inversion);
    EXPECT_TRUE(is_valid_permutation(route, n));
}

TEST(MutationValidity, ScrambleProducesValidPermutation) {
    std::mt19937 rng(42);
    const int n = 20;
    std::vector<int> route(n);
    std::iota(route.begin(), route.end(), 0);
    std::shuffle(route.begin(), route.end(), rng);

    std::mt19937 mut_rng(789);
    tsp::ga::mutate(route, n, mut_rng, tsp::cli::MutationType::Scramble);
    EXPECT_TRUE(is_valid_permutation(route, n));
}

TEST(MutationValidity, AllMutationsValidWithManySeeds) {
    const int n = 30;
    for (int seed = 1; seed <= 10; ++seed) {
        std::mt19937 rng(seed);
        std::vector<int> route(n);
        std::iota(route.begin(), route.end(), 0);
        std::shuffle(route.begin(), route.end(), rng);

        for (auto mut_type : {tsp::cli::MutationType::Swap,
                               tsp::cli::MutationType::Inversion,
                               tsp::cli::MutationType::Scramble}) {
            std::vector<int> mut_route = route;  // copy
            std::mt19937 mut_rng(seed + 500);
            tsp::ga::mutate(mut_route, n, mut_rng, mut_type);
            EXPECT_TRUE(is_valid_permutation(mut_route, n))
                << "Mutation failed with seed " << seed;
        }
    }
}

// =========================================================================
// Elitism Property Tests (Issue #5)
// =========================================================================
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
        tsp::ga::next_generation(pop, cities.data(), n, mutation_rate, rng,
                                  tsp::cli::CrossoverType::OX,
                                  tsp::cli::MutationType::Swap);
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

TEST(ElitismProperty, BestFitnessNeverWorsensWithPMXAndInversion) {
    std::mt19937 rng(99);
    const int n = 12, pop_size = 40, generations = 40;
    const double mutation_rate = 0.05;

    std::vector<tsp::City> cities(n);
    tsp::init_cities_random(cities.data(), n, rng);

    auto pop = tsp::ga::init_population(cities.data(), n, pop_size, rng);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    double prev_best = std::numeric_limits<double>::infinity();
    for (const auto& r : pop.routes) {
        if (r.fitness < prev_best) prev_best = r.fitness;
    }

    for (int gen = 0; gen < generations; ++gen) {
        tsp::ga::next_generation(pop, cities.data(), n, mutation_rate, rng,
                                  tsp::cli::CrossoverType::PMX,
                                  tsp::cli::MutationType::Inversion);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);

        double curr_best = std::numeric_limits<double>::infinity();
        for (const auto& r : pop.routes) {
            if (r.fitness < curr_best) curr_best = r.fitness;
        }

        EXPECT_LE(curr_best, prev_best + 1e-9)
            << "Elitism failed with PMX+Inversion at gen " << gen;

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
        tsp::ga::next_generation(pop, cities.data(), n, mutation_rate, rng,
                                  tsp::cli::CrossoverType::OX,
                                  tsp::cli::MutationType::Swap);
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

// =========================================================================
// CSV Loader Tests (Issue #7)
// =========================================================================
TEST(CsvLoader, LoadValidCsv) {
    // Create a temporary CSV file
    const char* temp_file = "/tmp/tsp_test_cities.csv";
    {
        std::ofstream f(temp_file);
        f << "x,y\n";
        f << "100,200\n";
        f << "300,400\n";
        f << "500,600\n";
    }

    std::vector<tsp::City> cities;
    EXPECT_TRUE(tsp::load_cities_from_csv(temp_file, cities));
    EXPECT_EQ(cities.size(), 3u);
    EXPECT_DOUBLE_EQ(cities[0].x, 100.0);
    EXPECT_DOUBLE_EQ(cities[0].y, 200.0);
    EXPECT_DOUBLE_EQ(cities[1].x, 300.0);
    EXPECT_DOUBLE_EQ(cities[1].y, 400.0);
    EXPECT_DOUBLE_EQ(cities[2].x, 500.0);
    EXPECT_DOUBLE_EQ(cities[2].y, 600.0);

    std::remove(temp_file);
}

TEST(CsvLoader, RejectsTooFewCities) {
    const char* temp_file = "/tmp/tsp_test_few.csv";
    {
        std::ofstream f(temp_file);
        f << "x,y\n";
        f << "100,200\n";
    }

    std::vector<tsp::City> cities;
    EXPECT_FALSE(tsp::load_cities_from_csv(temp_file, cities));

    std::remove(temp_file);
}

TEST(CsvLoader, RejectsMissingFile) {
    std::vector<tsp::City> cities;
    EXPECT_FALSE(tsp::load_cities_from_csv("/tmp/nonexistent_file_12345.csv", cities));
}

} // namespace
