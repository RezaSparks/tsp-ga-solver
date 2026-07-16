// population.h
#pragma once
#include "tsp/city.h"

typedef struct {
    int* route;
    double fitness;
} route_t;

typedef struct {
    route_t* routes;
    int size;
} population_t;

population_t* init_population(const city_t* cities, int n, int pop_size);
void free_population(population_t* pop);
void evaluate_fitness(population_t* pop, const city_t* cities, int n);
void tournament_selection(const population_t* pop, int* parent1, int* parent2);
void crossover(const int* p1, const int* p2, int* child, int n);
void mutate(int* route, int n);
void next_generation(population_t* pop, const city_t* cities, int n, double pm);