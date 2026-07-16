// city.cpp
#include "tsp/city.h"
#include <math.h>
#include <stdlib.h>

void init_cities(city_t* cities, int n) {
    for (int i = 0; i < n; i++) {
        cities[i].x = rand() % 800 + 50;
        cities[i].y = rand() % 600 + 50;
    }
}

double distance(const city_t* a, const city_t* b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    return sqrt((double)(dx * dx + dy * dy));
}