// src/city.cpp
#include "include/tsp/city.h"

#include <cmath>
#include <random>

namespace tsp {

void input_cities(City* cities, int n) {
    for (int i = 0; i < n; i++) {
        std::printf("Enter coordinates for city %d (x y): ", i + 1);
        int x = 0, y = 0;
        if (std::scanf(" (%d , %d)", &x, &y) != 2) {
            std::scanf("%d %d", &x, &y);
        }
        cities[i].x = x;
        cities[i].y = y;
    }
}

void init_cities_random(City* cities, int n, std::mt19937& rng) {
    std::uniform_int_distribution<int> x_dist(50, 850);
    std::uniform_int_distribution<int> y_dist(50, 650);
    for (int i = 0; i < n; i++) {
        cities[i].x = x_dist(rng);
        cities[i].y = y_dist(rng);
    }
}

double distance(const City* a, const City* b) {
    const int dx = a->x - b->x;
    const int dy = a->y - b->y;
    return std::sqrt(static_cast<double>(dx * dx + dy * dy));
}

} // namespace tsp