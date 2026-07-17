// include/tsp/city.h
#pragma once

#include <random>

namespace tsp {

struct City {
    int x = 0;
    int y = 0;
};

void input_cities(City* cities, int n);
void init_cities_random(City* cities, int n, std::mt19937& rng);
double distance(const City* a, const City* b);

} // namespace tsp