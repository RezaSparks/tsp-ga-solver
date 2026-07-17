// src/main.cpp
//
// Entry point. As of Issue #5, visualization is entirely optional: pass
// --headless to run the GA loop with no window, no Raylib calls, and no
// dependency on a display -- suitable for CI, SSH sessions, or scripted
// batch runs (Milestone 6 builds on exactly this).
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

// Issue #6: prints both the best distance found AND the actual route
// (as visited city indices + their coordinates), not just the distance.
// Kept as its own function so it's easy to unit-test later (Milestone 5)
// and easy to swap for a file-writer in Milestone 2 ("Save results",
// "Export the final route") without touching main()'s control flow.
void print_final_results(const tsp::City* cities, const tsp::ga::Route& best_route, int n) {
    std::printf("\n===== Final Results =====\n");
    std::printf("Cities:        %d\n", n);
    std::printf("Best distance: %.2f\n", best_route.fitness);
    std::printf("\nBest route (visiting order, returns to the start at the end):\n");

    for (int i = 0; i < n; i++) {
        const int city_index = best_route.route[i];
        std::printf("  %3d: city %-3d (%d, %d)\n",
                    i, city_index, cities[city_index].x, cities[city_index].y);
    }
    // Show the closing edge back to the starting city explicitly, since
    // the tour is a cycle.
    const int start_index = best_route.route[0];
    std::printf("  %3d: city %-3d (%d, %d)  <- back to start\n",
                n, start_index, cities[start_index].x, cities[start_index].y);
}
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

    if (!config.headless) {
        InitWindow(kWindowWidth, kWindowHeight, "TSP Genetic Algorithm");
        SetTargetFPS(60);
    }

    int gen = 0;
    std::size_t best = 0;

    // ---------- MAIN GENETIC ALGORITHM LOOP ----------
    // In windowed mode this also stops early if the user closes the
    // window; in headless mode there's no window to close, so it just
    // runs for the requested number of generations.
    while (gen < config.generations && (config.headless || !WindowShouldClose())) {
        tsp::ga::next_generation(pop, cities.data(), config.num_cities, config.mutation_rate);
        tsp::ga::evaluate_fitness(pop, cities.data(), config.num_cities);
        gen++;

        best = 0;
        for (std::size_t i = 1; i < pop.routes.size(); i++) {
            if (pop.routes[i].fitness < pop.routes[best].fitness) best = i;
        }

        if (!config.headless) {
            BeginDrawing();
            ClearBackground(BLACK);
            tsp::visualization::draw_route(cities.data(), pop.routes[best].route.data(),
                                            config.num_cities, gen);
            EndDrawing();
        }
    }

    // Recompute the best route once more in case the loop above exited
    // at gen == 0 (e.g. --generations 0 wasn't rejected by validation --
    // it is, actually, since ParseStatus::ExitError requires >= 1, but
    // this stays correct even if that check ever changes).
    best = 0;
    for (std::size_t i = 1; i < pop.routes.size(); i++) {
        if (pop.routes[i].fitness < pop.routes[best].fitness) best = i;
    }

    if (!config.headless) {
        // ---------- FINAL SCREEN: show the best solution and wait ----------
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
    }

    print_final_results(cities.data(), pop.routes[best], config.num_cities);

    return 0;
}
