// src/cli.cpp
#include "include/cli/options.h"

#include <cxxopts.hpp>

#include <iostream>
#include <string>

namespace tsp::cli {

    static std::string crossover_to_string(CrossoverType c) {
        switch (c) {
            case CrossoverType::OX:     return "ox";
            case CrossoverType::PMX:    return "pmx";
            case CrossoverType::Cycle:  return "cycle";
        }
        return "ox";
    }

    static std::string mutation_to_string(MutationType m) {
        switch (m) {
            case MutationType::Swap:      return "swap";
            case MutationType::Inversion: return "inversion";
            case MutationType::Scramble:  return "scramble";
        }
        return "swap";
    }

    ParseResult parse_args(int argc, char** argv) {
        ParseResult result;
        result.status = ParseStatus::ExitError;

        const Config defaults;

        try {
            cxxopts::Options options("tsp_solver",
                                     "Genetic Algorithm solver for the Traveling Salesman Problem");

            options.add_options()
                ("n,cities", "Number of cities to generate (ignored if --tsplib or --csv is used)",
                    cxxopts::value<int>()->default_value(std::to_string(defaults.num_cities)))
                ("p,population", "Population size",
                    cxxopts::value<int>()->default_value(std::to_string(defaults.population_size)))
                ("g,generations", "Number of generations to run",
                    cxxopts::value<int>()->default_value(std::to_string(defaults.generations)))
                ("m,mutation-rate", "Mutation rate, between 0.0 and 1.0",
                    cxxopts::value<double>()->default_value(std::to_string(defaults.mutation_rate)))
                ("crossover", "Crossover operator: ox, pmx, cycle (default: ox)",
                    cxxopts::value<std::string>()->default_value("ox"))
                ("mutation", "Mutation operator: swap, inversion, scramble (default: swap)",
                    cxxopts::value<std::string>()->default_value("swap"))
                ("output-csv", "Export convergence data to CSV file",
                    cxxopts::value<std::string>()->default_value(""))
                ("tsplib", "Load cities from a TSPLIB .tsp file (EUC_2D only)",
                    cxxopts::value<std::string>()->default_value(""))
                ("csv", "Load cities from a CSV file (header: x,y; one city per line)",
                    cxxopts::value<std::string>()->default_value(""))
                ("runs", "Number of independent runs for benchmarking",
                    cxxopts::value<int>()->default_value(std::to_string(defaults.runs)))
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

            // Parse crossover type
            {
                std::string cx = parsed["crossover"].as<std::string>();
                if (cx == "ox" || cx == "OX") {
                    result.config.crossover = CrossoverType::OX;
                } else if (cx == "pmx" || cx == "PMX") {
                    result.config.crossover = CrossoverType::PMX;
                } else if (cx == "cycle" || cx == "CYCLE" || cx == "cx" || cx == "CX") {
                    result.config.crossover = CrossoverType::Cycle;
                } else {
                    std::cerr << "Error: unknown crossover type '" << cx
                              << "'. Supported: ox, pmx, cycle\n";
                    result.status = ParseStatus::ExitError;
                    return result;
                }
            }

            // Parse mutation type
            {
                std::string mut = parsed["mutation"].as<std::string>();
                if (mut == "swap" || mut == "SWAP") {
                    result.config.mutation = MutationType::Swap;
                } else if (mut == "inversion" || mut == "INVERSION" || mut == "inv" || mut == "INV") {
                    result.config.mutation = MutationType::Inversion;
                } else if (mut == "scramble" || mut == "SCRAMBLE" || mut == "scr" || mut == "SCR") {
                    result.config.mutation = MutationType::Scramble;
                } else {
                    std::cerr << "Error: unknown mutation type '" << mut
                              << "'. Supported: swap, inversion, scramble\n";
                    result.status = ParseStatus::ExitError;
                    return result;
                }
            }

            result.config.num_cities = parsed["cities"].as<int>();
            result.config.population_size = parsed["population"].as<int>();
            result.config.generations = parsed["generations"].as<int>();
            result.config.mutation_rate = parsed["mutation-rate"].as<double>();
            result.config.headless = parsed.count("headless") > 0;
            result.config.seed = parsed["seed"].as<unsigned int>();
            result.config.output_csv = parsed["output-csv"].as<std::string>();
            result.config.tsplib_file = parsed["tsplib"].as<std::string>();
            result.config.csv_file = parsed["csv"].as<std::string>();
            result.config.runs = parsed["runs"].as<int>();

        } catch (const cxxopts::exceptions::exception& e) {
            std::cerr << "Error parsing arguments: " << e.what() << "\n\n";
            result.status = ParseStatus::ExitError;
            return result;
        }

        // Validation
        if (result.config.num_cities < 3) {
            std::cerr << "Error: --cities must be at least 3 (got "
                      << result.config.num_cities << ")\n";
            return result;
        }
        if (result.config.population_size < 4) {
            std::cerr << "Error: --population must be at least 4 (got "
                      << result.config.population_size << "). "
                      << "Elitism reserves 2 slots for the best routes, so anything "
                      << "below 4 leaves no room for actual offspring -- the GA would "
                      << "run for as many generations as you like without ever "
                      << "evolving.\n";
            return result;
        }
        if (result.config.generations < 1) {
            std::cerr << "Error: --generations must be at least 1 (got "
                      << result.config.generations << ")\n";
            return result;
        }
        if (result.config.mutation_rate < 0.0 || result.config.mutation_rate > 1.0) {
            std::cerr << "Error: --mutation-rate must be between 0.0 and 1.0 (got "
                      << result.config.mutation_rate << ")\n";
            return result;
        }
        if (result.config.runs < 1) {
            std::cerr << "Error: --runs must be at least 1 (got "
                      << result.config.runs << ")\n";
            return result;
        }

        // Cannot use both --tsplib and --csv
        if (!result.config.tsplib_file.empty() && !result.config.csv_file.empty()) {
            std::cerr << "Error: --tsplib and --csv are mutually exclusive\n";
            return result;
        }

        result.status = ParseStatus::Success;
        return result;
    }

} // namespace tsp::cli
