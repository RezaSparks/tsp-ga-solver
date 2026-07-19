// src/renderer.cpp
#include "include/visualization/renderer.h"

#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

namespace tsp::visualization {

static const int SCREEN_WIDTH = 900;
static const int SCREEN_HEIGHT = 700;
static const int CITY_RADIUS = 8;

void init_window(int /*num_cities*/) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TSP Genetic Algorithm Solver");
    SetTargetFPS(60);
}

Viewport compute_viewport(const City* cities, int num_cities) {
    if (num_cities <= 0) return Viewport{};

    double min_x = std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    double max_x = -std::numeric_limits<double>::infinity();
    double max_y = -std::numeric_limits<double>::infinity();

    for (int i = 0; i < num_cities; ++i) {
        min_x = std::min(min_x, cities[i].x);
        min_y = std::min(min_y, cities[i].y);
        max_x = std::max(max_x, cities[i].x);
        max_y = std::max(max_y, cities[i].y);
    }

    Viewport vp;
    vp.min_x = min_x;
    vp.min_y = min_y;
    vp.padding = 20;

    double data_width = max_x - min_x;
    double data_height = max_y - min_y;

    // Avoid division by zero for single-point or axis-aligned data
    if (data_width < 1e-9) data_width = 1.0;
    if (data_height < 1e-9) data_height = 1.0;

    int available_w = SCREEN_WIDTH - 2 * vp.padding;
    int available_h = SCREEN_HEIGHT - 2 * vp.padding;

    double scale_x = static_cast<double>(available_w) / data_width;
    double scale_y = static_cast<double>(available_h) / data_height;
    vp.scale = std::min(scale_x, scale_y);  // maintain aspect ratio

    return vp;
}

static int to_screen_x(double x, const Viewport& vp) {
    return static_cast<int>((x - vp.min_x) * vp.scale + vp.padding);
}

static int to_screen_y(double y, const Viewport& vp) {
    // Flip Y so (0,0) is bottom-left (matches typical math coordinates)
    int available_h = SCREEN_HEIGHT - 2 * vp.padding;
    double data_height = available_h / vp.scale;
    return static_cast<int>(SCREEN_HEIGHT - vp.padding - (y - vp.min_y) * vp.scale);
}

void draw_route(const City* cities, int num_cities, const int* route,
                int generation, int max_generations) {
    Viewport vp = compute_viewport(cities, num_cities);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Title
    char title[128];
    snprintf(title, sizeof(title), "Generation: %d / %d", generation + 1, max_generations);
    DrawText(title, 10, 10, 20, DARKGRAY);

    // Draw route lines
    for (int i = 0; i < num_cities - 1; ++i) {
        int a = route[i];
        int b = route[i + 1];
        DrawLine(
            to_screen_x(cities[a].x, vp), to_screen_y(cities[a].y, vp),
            to_screen_x(cities[b].x, vp), to_screen_y(cities[b].y, vp),
            DARKGRAY
        );
    }
    // Closing edge
    int last = route[num_cities - 1];
    int first = route[0];
    DrawLine(
        to_screen_x(cities[last].x, vp), to_screen_y(cities[last].y, vp),
        to_screen_x(cities[first].x, vp), to_screen_y(cities[first].y, vp),
        DARKGRAY
    );

    // Draw cities
    for (int i = 0; i < num_cities; ++i) {
        Color color = (i == route[0]) ? RED : BLUE;
        int sx = to_screen_x(cities[i].x, vp);
        int sy = to_screen_y(cities[i].y, vp);
        DrawCircle(sx, sy, CITY_RADIUS, color);

        // City label
        char label[16];
        snprintf(label, sizeof(label), "%d", i);
        DrawText(label, sx + 10, sy - 5, 14, DARKGRAY);
    }

    // Draw scale info
    char scale_info[64];
    snprintf(scale_info, sizeof(scale_info), "Scale: %.2fpx/unit", vp.scale);
    DrawText(scale_info, 10, SCREEN_HEIGHT - 30, 16, GRAY);

    EndDrawing();
}

bool should_close() {
    return WindowShouldClose();
}

void wait_for_close() {
    while (!WindowShouldClose()) {
        WaitTime(0.1);
    }
}

void close_window() {
    CloseWindow();
}

} // namespace tsp::visualization
