// include/cli/options.h
//
// Command-line argument parsing.
// Issue #10: --crossover flag (ox|pmx|cycle)
// Issue #11: --mutation flag (swap|inversion|scramble)
// Issue #12: --output-csv flag
// Issue #13: --runs flag
// Issue #8:  --tsplib and --csv flags
#pragma once

#include <string>

namespace tsp::cli {

    enum class CrossoverType { OX, PMX, Cycle };
    enum class MutationType { Swap, Inversion, Scramble };

    struct Config {
        int num_cities = 20;
        int population_size = 100;
        int generations = 500;
        double mutation_rate = 0.02;
        bool headless = false;
        unsigned int seed = 0;  // 0 = auto, otherwise fixed

        // Issue #10
        CrossoverType crossover = CrossoverType::OX;
        // Issue #11
        MutationType mutation = MutationType::Swap;
        // Issue #12
        std::string output_csv;  // empty = no CSV output
        // Issue #8
        std::string tsplib_file;  // empty = generate random cities
        std::string csv_file;     // empty = generate random cities
        // Issue #13
        int runs = 1;  // number of independent runs for benchmarking
    };

    enum class ParseStatus {
        Success,
        ExitOk,
        ExitError,
    };

    struct ParseResult {
        ParseStatus status = ParseStatus::ExitError;
        Config config;
    };

    ParseResult parse_args(int argc, char** argv);

} // namespace tsp::cli
