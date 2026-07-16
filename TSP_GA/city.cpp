#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include "tsp/city.h"
#include <math.h>
#include <stdlib.h>

void input_cities(city_t* cities, int n) {
    int x, y;
    for (int i = 0; i < n; i++) {
        printf("Enter coordinates for city %d (x y): ", i + 1);

        // Try to read with parentheses, e.g. (12, 34)
        if (scanf(" (%d , %d)", &x, &y) != 2) {
            // If that fails (user typed without parentheses), read plain numbers
            scanf("%d %d", &x, &y);
        }
        cities[i].x = x;
        cities[i].y = y;
    }
}

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