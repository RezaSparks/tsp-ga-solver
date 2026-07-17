// src/main.cpp
//
// Entry point. As of Issue #4, all GA parameters come from CLI flags
// instead of interactive scanf prompts, and cities are randomly generated
// (file-based input is Milestone 2, "Load data from files"). Visualization
// is still always-on for now -- toggling it off via a flag is Issue #5.
#include <raylib.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "include/cli/options.h"
#include "include/ga/population.h"
#include "include/tsp/city.h"
#include "include/visualization/renderer.h"

namespace {
constexpr int kWindowWidth = 900;
constexpr int kWindowHeight = 700;
} // namespace

int main(int argc, char** argv) {
    const tsp::cli::ParseResult parsed = tsp::cli::parse_args(argc, argv);
    if (parsed.status == tsp::cli::ParseStatus::ExitOk) return 0;
    if (parsed.status == tsp::cli::ParseStatus::ExitError) return 1;
    const tsp::cli::Config& config = parsed.config;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<tsp::City> cities(config.num_cities);
    tsp::init_cities_random(cities.data(), config.num_cities);

    tsp::ga::Population pop =
        tsp::ga::init_population(cities.data(), config.num_cities, config.population_size);
    tsp::ga::evaluate_fitness(pop, cities.data(), config.num_cities);

    InitWindow(kWindowWidth, kWindowHeight, "TSP Genetic Algorithm");
    SetTargetFPS(60);

    int gen = 0;
    std::size_t best = 0;

    // ---------- MAIN GENETIC ALGORITHM LOOP ----------
    while (!WindowShouldClose() && gen < config.generations) {
        tsp::ga::next_generation(pop, cities.data(), config.num_cities, config.mutation_rate);
        tsp::ga::evaluate_fitness(pop, cities.data(), config.num_cities);
        gen++;

        best = 0;
        for (std::size_t i = 1; i < pop.routes.size(); i++) {
            if (pop.routes[i].fitness < pop.routes[best].fitness) best = i;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        tsp::visualization::draw_route(cities.data(), pop.routes[best].route.data(),
                                        config.num_cities, gen);
        EndDrawing();
    }

    // ---------- FINAL SCREEN: show the best solution and wait ----------
    best = 0;
    for (std::size_t i = 1; i < pop.routes.size(); i++) {
        if (pop.routes[i].fitness < pop.routes[best].fitness) best = i;
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        tsp::visualization::draw_route(cities.data(), pop.routes[best].route.data(),
                                        config.num_cities, gen);
        DrawText("FINAL SOLUTION - Press any key or ESC to close", 10, 30, 20, YELLOW);
        EndDrawing();

        if (GetKeyPressed() != 0) break;
        WaitTime(0.1);
    }

    CloseWindow();

    std::printf("\nBest route length found: %.2f\n", pop.routes[best].fitness);

    return 0;
}
