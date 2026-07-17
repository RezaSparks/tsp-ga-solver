// include/cli/options.h
#pragma once

namespace tsp::cli {

struct Config {
    int num_cities = 20;
    int population_size = 100;
    int generations = 500;
    double mutation_rate = 0.02;
    bool headless = false;
    unsigned int seed = 0;  // 0 = use random_device, otherwise fixed seed
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