#pragma once

typedef struct {
    int x, y;
} city_t;

void input_cities(city_t* cities, int n);   
double distance(const city_t* a, const city_t* b);