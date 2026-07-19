// src/renderer.cpp
#include "include/visualization/renderer.h"

#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace tsp::visualization {

static const int SCREEN_WIDTH = 900;
static const int SCREEN_HEIGHT = 700;
static const int CITY_RADIUS = 8;

void init_window(int /*num_cities*/) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TSP Genetic Algorithm Solver");
    SetTargetFPS(60);
}

// BUGFIX: city coordinates were previously cast straight to pixel
// coordinates. That's fine for the --cities (randomly generated) path,
// since those are already generated within the window's bounds -- but
// real TSPLIB instances use whatever coordinate system the original
// dataset used. berlin52, for example, ranges up to x=1740, y=1175,
// while the window is 900x700: roughly half its cities would render
// completely off-screen, and the tour would look broken. This computes
// a uniform scale factor (preserving aspect ratio, so the shape of the
// tour isn't distorted) plus a small margin, and fits every instance to
// the window regardless of its native coordinate range.
namespace {
    struct Fit {
        double scale = 1.0;
        double min_x = 0.0;
        double min_y = 0.0;
    };

    Fit compute_fit(const City* cities, int num_cities) {
        constexpr int kMargin = 40;
        double min_x = cities[0].x, max_x = cities[0].x;
        double min_y = cities[0].y, max_y = cities[0].y;
        for (int i = 1; i < num_cities; ++i) {
            min_x = std::min(min_x, cities[i].x);
            max_x = std::max(max_x, cities[i].x);
            min_y = std::min(min_y, cities[i].y);
            max_y = std::max(max_y, cities[i].y);
        }
        const double span_x = std::max(max_x - min_x, 1.0);
        const double span_y = std::max(max_y - min_y, 1.0);
        const double avail_w = static_cast<double>(SCREEN_WIDTH - 2 * kMargin);
        const double avail_h = static_cast<double>(SCREEN_HEIGHT - 2 * kMargin);
        const double scale = std::min(avail_w / span_x, avail_h / span_y);
        return { scale, min_x, min_y };
    }

    int to_screen_x(const Fit& fit, double x) {
        return static_cast<int>((x - fit.min_x) * fit.scale) + 40;
    }
    int to_screen_y(const Fit& fit, double y) {
        return static_cast<int>((y - fit.min_y) * fit.scale) + 40;
    }
} // namespace

void draw_route(const City* cities, int num_cities, const int* route, int generation, int max_generations) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    char title[128];
    snprintf(title, sizeof(title), "Generation: %d / %d", generation + 1, max_generations);
    DrawText(title, 10, 10, 20, DARKGRAY);
    DrawText("Press ESC or close the window to exit", 10, SCREEN_HEIGHT - 30, 16, GRAY);

    const Fit fit = compute_fit(cities, num_cities);

    // Draw all cities as points
    for (int i = 0; i < num_cities; ++i) {
        Color color = (i == route[0]) ? RED : BLUE;
        const int sx = to_screen_x(fit, cities[i].x);
        const int sy = to_screen_y(fit, cities[i].y);
        DrawCircle(sx, sy, CITY_RADIUS, color);
        char label[16];
        snprintf(label, sizeof(label), "%d", i);
        DrawText(label, sx + 10, sy - 5, 14, DARKGRAY);
    }

    // Draw the route
    for (int i = 0; i < num_cities - 1; ++i) {
        int idx1 = route[i];
        int idx2 = route[i + 1];
        DrawLine(to_screen_x(fit, cities[idx1].x), to_screen_y(fit, cities[idx1].y),
                 to_screen_x(fit, cities[idx2].x), to_screen_y(fit, cities[idx2].y), DARKGRAY);
    }
    int last = route[num_cities - 1];
    int first = route[0];
    DrawLine(to_screen_x(fit, cities[last].x), to_screen_y(fit, cities[last].y),
             to_screen_x(fit, cities[first].x), to_screen_y(fit, cities[first].y), DARKGRAY);

    EndDrawing();
}

bool should_close() {
    return WindowShouldClose();
}

// BUGFIX: this previously only called WaitTime(0.1) in a loop. Raylib only
// pumps window/input events inside EndDrawing() (PollInputEvents() is
// called from there, not from WindowShouldClose() itself) -- so a loop
// that never calls BeginDrawing()/EndDrawing() or PollInputEvents() can
// never observe a close-button click or ESC key press. WindowShouldClose()
// would return false forever, and since the OS message queue for the
// window is never drained either, the window itself is reported as "Not
// Responding" by the OS. Explicitly polling here fixes both symptoms.
void wait_for_close() {
    while (!WindowShouldClose()) {
        PollInputEvents();
        WaitTime(0.1);
    }
}

void close_window() {
    CloseWindow();
}

} // namespace tsp::visualization
