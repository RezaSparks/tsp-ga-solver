// Main.cpp
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

#include "tsp/city.h"
#include "ga/population.h"
#include "visualization/renderer.h"


#define WIN_W 900
#define WIN_H 700

int main() {
    srand((unsigned int)time(NULL));
    int n, pop_size, generations;
    double mutation_rate;

    printf("Enter number of cities: ");
    scanf("%d", &n);
    printf("Population size: ");
    scanf("%d", &pop_size);
    printf("Generations: ");
    scanf("%d", &generations);
    printf("Mutation rate (0-1): ");
    scanf("%lf", &mutation_rate);

    city_t* cities = (city_t*)malloc(n * sizeof(city_t));
    input_cities(cities, n);   // <-- uses the new parenthesized input

    population_t* pop = init_population(cities, n, pop_size);
    evaluate_fitness(pop, cities, n);

    InitWindow(WIN_W, WIN_H, "TSP Genetic Algorithm");
    SetTargetFPS(60);

    int gen = 0;
    int best = 0; // will store index of best route

    // ---------- MAIN GENETIC ALGORITHM LOOP ----------
    while (!WindowShouldClose() && gen < generations) {
        next_generation(pop, cities, n, mutation_rate);
        evaluate_fitness(pop, cities, n);
        gen++;

        // Find best route of this generation
        best = 0;
        for (int i = 1; i < pop->size; i++) {
            if (pop->routes[i].fitness < pop->routes[best].fitness)
                best = i;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        draw_route(cities, pop->routes[best].route, n, gen);
        EndDrawing();
    }

    // ---------- FINAL SCREEN: show the best solution and wait ----------
    // Re-find best one last time (in case loop exited due to window close)
    best = 0;
    for (int i = 1; i < pop->size; i++) {
        if (pop->routes[i].fitness < pop->routes[best].fitness)
            best = i;
    }

    // Display final result and hold the window open
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        draw_route(cities, pop->routes[best].route, n, gen);
        DrawText("FINAL SOLUTION - Press any key or ESC to close", 10, 30, 20, YELLOW);
        EndDrawing();

        if (GetKeyPressed() != 0) break;  // any key closes the graphics window
        WaitTime(0.1); // prevent 100% CPU usage
    }

    CloseWindow();

    // ---------- CONSOLE PAUSE ----------
    // Print the best length and wait for Enter
    printf("\nBest route length found: %.2f\n", pop->routes[best].fitness);
    printf("Press Enter to close this console...");
    getchar(); // consume leftover newline from previous scanf
    getchar(); // wait for actual Enter press

    free(cities);
    free_population(pop);
    return 0;
}