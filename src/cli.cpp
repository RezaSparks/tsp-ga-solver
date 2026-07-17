// src/cli.cpp
#include "include/cli/options.h"

#include <cxxopts.hpp>

#include <iostream>
#include <string>

namespace tsp::cli {

ParseResult parse_args(int argc, char** argv) {
    Config defaults; // used to seed cxxopts' default_value strings below

    cxxopts::Options options(
        "tsp_solver",
        "A genetic-algorithm solver for the Traveling Salesman Problem, "
        "with optional real-time Raylib visualization."
    );

    options.add_options()
        ("n,cities", "Number of cities to generate",
            cxxopts::value<int>()->default_value(std::to_string(defaults.num_cities)))
        ("p,population", "Population size",
            cxxopts::value<int>()->default_value(std::to_string(defaults.population_size)))
        ("g,generations", "Number of generations to run",
            cxxopts::value<int>()->default_value(std::to_string(defaults.generations)))
        ("m,mutation-rate", "Mutation rate, between 0.0 and 1.0",
            cxxopts::value<double>()->default_value(std::to_string(defaults.mutation_rate)))
        ("headless", "Run without opening a visualization window (Raylib GUI stays closed)")
        ("h,help", "Print usage and exit");

    ParseResult result;

    try {
        auto parsed = options.parse(argc, argv);

        if (parsed.count("help")) {
            std::cout << options.help() << std::endl;
            result.status = ParseStatus::ExitOk;
            return result;
        }

        result.config.num_cities = parsed["cities"].as<int>();
        result.config.population_size = parsed["population"].as<int>();
        result.config.generations = parsed["generations"].as<int>();
        result.config.mutation_rate = parsed["mutation-rate"].as<double>();
        result.config.headless = parsed.count("headless") > 0;
    } catch (const cxxopts::exceptions::exception& e) {
        // Covers things like "--cities notanumber" or an unknown flag.
        std::cerr << "Error parsing arguments: " << e.what() << "\n\n";
        std::cerr << options.help() << std::endl;
        result.status = ParseStatus::ExitError;
        return result;
    }

    // Range checks cxxopts itself doesn't do (it happily accepts negative
    // ints or a mutation rate of 5.0).
    if (result.config.num_cities < 3) {
        std::cerr << "Error: --cities must be at least 3 (got "
                  << result.config.num_cities << ")\n";
        result.status = ParseStatus::ExitError;
        return result;
    }
    if (result.config.population_size < 2) {
        std::cerr << "Error: --population must be at least 2 (got "
                  << result.config.population_size << ")\n";
        result.status = ParseStatus::ExitError;
        return result;
    }
    if (result.config.generations < 1) {
        std::cerr << "Error: --generations must be at least 1 (got "
                  << result.config.generations << ")\n";
        result.status = ParseStatus::ExitError;
        return result;
    }
    if (result.config.mutation_rate < 0.0 || result.config.mutation_rate > 1.0) {
        std::cerr << "Error: --mutation-rate must be between 0.0 and 1.0 (got "
                  << result.config.mutation_rate << ")\n";
        result.status = ParseStatus::ExitError;
        return result;
    }

    result.status = ParseStatus::Success;
    return result;
}

} // namespace tsp::cli
