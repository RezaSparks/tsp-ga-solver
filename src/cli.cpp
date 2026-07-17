// src/cli.cpp
#include "include/cli/options.h"

#include <cxxopts.hpp>
#include <iostream>
#include <string>

namespace tsp::cli {

ParseResult parse_args(int argc, char** argv) {
    ParseResult result;
    result.status = ParseStatus::ExitError;

    const Config defaults;

    try {
        cxxopts::Options options("tsp_solver",
                                 "Genetic Algorithm solver for the Traveling Salesman Problem");

        options.add_options()
            ("n,cities", "Number of cities to generate",
                cxxopts::value<int>()->default_value(std::to_string(defaults.num_cities)))
            ("p,population", "Population size",
                cxxopts::value<int>()->default_value(std::to_string(defaults.population_size)))
            ("g,generations", "Number of generations to run",
                cxxopts::value<int>()->default_value(std::to_string(defaults.generations)))
            ("m,mutation-rate", "Mutation rate, between 0.0 and 1.0",
                cxxopts::value<double>()->default_value(std::to_string(defaults.mutation_rate)))
            ("headless", "Run without Raylib visualization (no window)",
                cxxopts::value<bool>()->default_value("false"))
            ("seed", "Random seed (0 = auto, any other value = fixed for reproducibility)",
                cxxopts::value<unsigned int>()->default_value(std::to_string(defaults.seed)))
            ("h,help", "Print usage and exit");

        auto parsed = options.parse(argc, argv);

        if (parsed.count("help")) {
            std::cout << options.help() << std::endl;
            result.status = ParseStatus::ExitOk;
            return result;
        }

        result.config.num_cities = parsed["cities"].as<int>();
        if (result.config.num_cities < 3) {
            std::cerr << "Error: --cities must be at least 3 (got " << result.config.num_cities << ")" << std::endl;
            std::cerr << options.help() << std::endl;
            return result;
        }

        result.config.population_size = parsed["population"].as<int>();
        if (result.config.population_size < 2) {
            std::cerr << "Error: --population must be at least 2 (got " << result.config.population_size << ")" << std::endl;
            std::cerr << options.help() << std::endl;
            return result;
        }

        result.config.generations = parsed["generations"].as<int>();
        if (result.config.generations < 1) {
            std::cerr << "Error: --generations must be at least 1 (got " << result.config.generations << ")" << std::endl;
            std::cerr << options.help() << std::endl;
            return result;
        }

        result.config.mutation_rate = parsed["mutation-rate"].as<double>();
        if (result.config.mutation_rate < 0.0 || result.config.mutation_rate > 1.0) {
            std::cerr << "Error: --mutation-rate must be between 0.0 and 1.0 (got " << result.config.mutation_rate << ")" << std::endl;
            std::cerr << options.help() << std::endl;
            return result;
        }

        result.config.headless = parsed["headless"].as<bool>();
        result.config.seed = parsed["seed"].as<unsigned int>();

        result.status = ParseStatus::Success;
        return result;

    } catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Usage: " << argv[0] << " [options]" << std::endl;
        return result;
    }
}

} // namespace tsp::cli