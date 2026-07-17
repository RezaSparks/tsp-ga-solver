// src/city.cpp
#include "include/tsp/city.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace tsp {

    void input_cities(City* cities, int n) {
        for (int i = 0; i < n; i++) {
            std::printf("Enter coordinates for city %d (x y): ", i + 1);

            int x = 0, y = 0;
            // Try to read with parentheses, e.g. (12, 34)
            if (std::scanf(" (%d , %d)", &x, &y) != 2) {
                // If that fails (user typed without parentheses), read plain numbers
                std::scanf("%d %d", &x, &y);
            }
            cities[i].x = x;
            cities[i].y = y;
        }
    }

    void init_cities_random(City* cities, int n) {
        for (int i = 0; i < n; i++) {
            cities[i].x = std::rand() % 800 + 50;
            cities[i].y = std::rand() % 600 + 50;
        }
    }

    double distance(const City* a, const City* b) {
        const int dx = a->x - b->x;
        const int dy = a->y - b->y;
        return std::sqrt(static_cast<double>(dx * dx + dy * dy));
    }

} // namespace tsp
