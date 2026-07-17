// src/main.cpp
//
// Entry point. For this milestone the behavior is intentionally identical
// to the original program (interactive prompts, always-on visualization):
// CLI arguments and a headless mode are separate Milestone 1 checklist
// items that will replace this interactive flow in a later step.
#include <raylib.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "include/ga/population.h"
#include "include/tsp/city.h"
#include "include/visualization/renderer.h"

namespace {
    constexpr int kWindowWidth = 900;
    constexpr int kWindowHeight = 700;
} // namespace

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int n = 0, pop_size = 0, generations = 0;
    double mutation_rate = 0.0;

    std::printf("Enter number of cities: ");
    std::scanf("%d", &n);
    std::printf("Population size: ");
    std::scanf("%d", &pop_size);
    std::printf("Generations: ");
    std::scanf("%d", &generations);
    std::printf("Mutation rate (0-1): ");
    std::scanf("%lf", &mutation_rate);

    std::vector<tsp::City> cities(n);
    tsp::input_cities(cities.data(), n);

    tsp::ga::Population pop = tsp::ga::init_population(cities.data(), n, pop_size);
    tsp::ga::evaluate_fitness(pop, cities.data(), n);

    InitWindow(kWindowWidth, kWindowHeight, "TSP Genetic Algorithm");
    SetTargetFPS(60);

    int gen = 0;
    std::size_t best = 0;

    // ---------- MAIN GENETIC ALGORITHM LOOP ----------
    while (!WindowShouldClose() && gen < generations) {
        tsp::ga::next_generation(pop, cities.data(), n, mutation_rate);
        tsp::ga::evaluate_fitness(pop, cities.data(), n);
        gen++;

        best = 0;
        for (std::size_t i = 1; i < pop.routes.size(); i++) {
            if (pop.routes[i].fitness < pop.routes[best].fitness) best = i;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        tsp::visualization::draw_route(cities.data(), pop.routes[best].route.data(), n, gen);
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
        tsp::visualization::draw_route(cities.data(), pop.routes[best].route.data(), n, gen);
        DrawText("FINAL SOLUTION - Press any key or ESC to close", 10, 30, 20, YELLOW);
        EndDrawing();

        if (GetKeyPressed() != 0) break;
        WaitTime(0.1);
    }

    CloseWindow();

    std::printf("\nBest route length found: %.2f\n", pop.routes[best].fitness);
    std::printf("Press Enter to close this console...");
    std::getchar();
    std::getchar();

    return 0;
}
