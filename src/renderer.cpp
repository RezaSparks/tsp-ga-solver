// src/renderer.cpp
#include "include/visualization/renderer.h"

#include <raylib.h>

#include <cstdio>

namespace tsp::visualization {

    void draw_route(const tsp::City* cities, const int* route, int n, int generation) {
        // Draw the tour as connected line segments.
        for (int i = 0; i < n - 1; i++) {
            int a = route[i], b = route[i + 1];
            DrawLine(cities[a].x, cities[a].y, cities[b].x, cities[b].y, GREEN);
        }
        DrawLine(cities[route[n - 1]].x, cities[route[n - 1]].y, cities[route[0]].x,
            cities[route[0]].y, GREEN);

        // Draw the cities themselves.
        for (int i = 0; i < n; i++) {
            DrawCircle(cities[i].x, cities[i].y, 5, RED);
        }

        char text[64];
        std::snprintf(text, sizeof(text), "Generation: %d", generation);
        DrawText(text, 10, 10, 20, WHITE);
    }

} // namespace tsp::visualization
