// src/renderer.cpp
#include "include/visualization/renderer.h"

#include <raylib.h>
#include <cmath>
#include <cstdio>

namespace tsp::visualization {

static const int SCREEN_WIDTH = 900;
static const int SCREEN_HEIGHT = 700;
static const int CITY_RADIUS = 8;
static const int LINE_WIDTH = 2;

void init_window(int num_cities) {
    (void)num_cities; // unused, just for API consistency
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TSP Genetic Algorithm Solver");
    SetTargetFPS(60);
}

void draw_route(const City* cities, int num_cities, const int* route, int generation, int max_generations) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw title
    char title[128];
    snprintf(title, sizeof(title), "Generation: %d / %d", generation + 1, max_generations);
    DrawText(title, 10, 10, 20, DARKGRAY);

    // Draw all cities as points
    for (int i = 0; i < num_cities; ++i) {
        Color color = (i == route[0]) ? RED : BLUE;
        DrawCircle(cities[i].x, cities[i].y, CITY_RADIUS, color);
        char label[16];
        snprintf(label, sizeof(label), "%d", i);
        DrawText(label, cities[i].x + 10, cities[i].y - 5, 14, DARKGRAY);
    }

    // Draw the route (lines between consecutive cities)
    for (int i = 0; i < num_cities - 1; ++i) {
        int idx1 = route[i];
        int idx2 = route[i + 1];
        DrawLine(cities[idx1].x, cities[idx1].y, cities[idx2].x, cities[idx2].y, DARKGRAY);
    }
    // Closing edge: back to start
    int last = route[num_cities - 1];
    int first = route[0];
    DrawLine(cities[last].x, cities[last].y, cities[first].x, cities[first].y, DARKGRAY);

    EndDrawing();
}

void wait_for_close() {
    // Wait for the user to press any key or close the window
    while (!WindowShouldClose()) {
        WaitTime(0.1);
    }
}

void close_window() {
    CloseWindow();
}

} // namespace tsp::visualization