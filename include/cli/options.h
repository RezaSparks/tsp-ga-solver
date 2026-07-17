// include/cli/options.h
//
// Command-line argument parsing (Issue #4). Deliberately kept as its own
// small module -- separate from ga/tsp/visualization -- because Issue #5
// (headless mode) and Issue #6 (printing the final route) both extend
// `Config` rather than touching argument parsing itself.
#pragma once

namespace tsp::cli {

// All GA parameters that used to be entered interactively via scanf.
// Extended by later issues: Issue #5 adds a `headless` flag here, Issue
// #6 doesn't need a new field (it just changes what main() prints).
struct Config {
    int num_cities = 20;
    int population_size = 100;
    int generations = 500;
    double mutation_rate = 0.02;
};

// What main() should do after parsing.
enum class ParseStatus {
    Success,     // `config` is valid, proceed normally.
    ExitOk,      // e.g. --help was requested; print nothing more, exit 0.
    ExitError,   // Invalid arguments; an error was already printed to stderr, exit 1.
};

struct ParseResult {
    ParseStatus status = ParseStatus::ExitError;
    Config config;
};

// Parses argc/argv (as passed to main) into a Config. Prints usage/help
// text or error messages to stdout/stderr as appropriate -- callers just
// need to check `status` and exit accordingly.
ParseResult parse_args(int argc, char** argv);

} // namespace tsp::cli
