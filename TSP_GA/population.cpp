// population.cpp
#include "ga/population.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int contains(const int* route, int n, int city) {
    for (int i = 0; i < n; i++) if (route[i] == city) return 1;
    return 0;
}

population_t* init_population(const city_t* cities, int n, int pop_size) {
    population_t* pop = (population_t*)malloc(sizeof(population_t));
    pop->size = pop_size;
    pop->routes = (route_t*)malloc(pop_size * sizeof(route_t));
    for (int i = 0; i < pop_size; i++) {
        pop->routes[i].route = (int*)malloc(n * sizeof(int));
        int* r = pop->routes[i].route;
        for (int j = 0; j < n; j++) r[j] = j;
        for (int j = n - 1; j > 0; j--) {
            int k = rand() % (j + 1);
            int tmp = r[j]; r[j] = r[k]; r[k] = tmp;
        }
        pop->routes[i].fitness = 0.0;
    }
    return pop;
}

void free_population(population_t* pop) {
    for (int i = 0; i < pop->size; i++) free(pop->routes[i].route);
    free(pop->routes);
    free(pop);
}

void evaluate_fitness(population_t* pop, const city_t* cities, int n) {
    for (int i = 0; i < pop->size; i++) {
        double total = 0.0;
        int* r = pop->routes[i].route;
        for (int j = 0; j < n - 1; j++) total += distance(&cities[r[j]], &cities[r[j + 1]]);
        total += distance(&cities[r[n - 1]], &cities[r[0]]);
        pop->routes[i].fitness = total;
    }
}

void tournament_selection(const population_t* pop, int* p1, int* p2) {
    int k = 3;
    int idx1 = rand() % pop->size;
    for (int i = 1; i < k; i++) {
        int c = rand() % pop->size;
        if (pop->routes[c].fitness < pop->routes[idx1].fitness) idx1 = c;
    }
    int idx2 = rand() % pop->size;
    for (int i = 1; i < k; i++) {
        int c = rand() % pop->size;
        if (pop->routes[c].fitness < pop->routes[idx2].fitness) idx2 = c;
    }
    *p1 = idx1; *p2 = idx2;
}

void crossover(const int* p1, const int* p2, int* child, int n) {
    int start = rand() % n, end = rand() % n;
    if (start > end) { int tmp = start; start = end; end = tmp; }
    int* used = (int*)calloc(n, sizeof(int));
    for (int i = start; i <= end; i++) {
        child[i] = p1[i];
        used[p1[i]] = 1;
    }
    int pos = 0;
    for (int i = 0; i < n; i++) {
        if (pos == start) pos = end + 1;
        if (pos >= n) break;
        if (!used[p2[i]]) {
            child[pos++] = p2[i];
            used[p2[i]] = 1;
        }
    }
    free(used);
}

void mutate(int* route, int n) {
    int i = rand() % n, j = rand() % n;
    int tmp = route[i]; route[i] = route[j]; route[j] = tmp;
}

void next_generation(population_t* pop, const city_t* cities, int n, double pm) {
    population_t new_pop;
    new_pop.size = pop->size;
    new_pop.routes = (route_t*)malloc(pop->size * sizeof(route_t));
    for (int i = 0; i < pop->size; i++) new_pop.routes[i].route = (int*)malloc(n * sizeof(int));

    // Elitism
    int best = 0, second = 1;
    for (int i = 1; i < pop->size; i++) {
        if (pop->routes[i].fitness < pop->routes[best].fitness) {
            second = best; best = i;
        }
        else if (pop->routes[i].fitness < pop->routes[second].fitness) second = i;
    }
    memcpy(new_pop.routes[0].route, pop->routes[best].route, n * sizeof(int));
    memcpy(new_pop.routes[1].route, pop->routes[second].route, n * sizeof(int));

    for (int i = 2; i < pop->size; i++) {
        int p1, p2;
        tournament_selection(pop, &p1, &p2);
        crossover(pop->routes[p1].route, pop->routes[p2].route, new_pop.routes[i].route, n);
        if ((double)rand() / RAND_MAX < pm) mutate(new_pop.routes[i].route, n);
    }

    for (int i = 0; i < pop->size; i++) {
        free(pop->routes[i].route);
        pop->routes[i].route = new_pop.routes[i].route;
    }
    free(new_pop.routes);
}