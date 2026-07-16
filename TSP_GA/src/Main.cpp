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
    init_cities(cities, n);

    population_t* pop = init_population(cities, n, pop_size);
    evaluate_fitness(pop, cities, n);

    InitWindow(WIN_W, WIN_H, "TSP Genetic Algorithm");
    SetTargetFPS(60);

    int gen = 0;
    while (!WindowShouldClose() && gen < generations) {
        next_generation(pop, cities, n, mutation_rate);
        evaluate_fitness(pop, cities, n);
        gen++;

        BeginDrawing();
        ClearBackground(BLACK);
        int best = 0;
        for (int i = 1; i < pop->size; i++)
            if (pop->routes[i].fitness < pop->routes[best].fitness)
                best = i;
        draw_route(cities, pop->routes[best].route, n, gen);
        EndDrawing();
    }

    CloseWindow();
    free(cities);
    free_population(pop);
    return 0;
}