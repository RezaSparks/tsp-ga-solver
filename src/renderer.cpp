// src/renderer.cpp
#include "include/visualization/renderer.h"

#include <raylib.h>
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

void draw_route(const City* cities, int num_cities, const int* route, int generation, int max_generations) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    char title[128];
    snprintf(title, sizeof(title), "Generation: %d / %d", generation + 1, max_generations);
    DrawText(title, 10, 10, 20, DARKGRAY);
    DrawText("Press ESC or close the window to exit", 10, SCREEN_HEIGHT - 30, 16, GRAY);

    // Draw all cities as points
    for (int i = 0; i < num_cities; ++i) {
        Color color = (i == route[0]) ? RED : BLUE;
        DrawCircle(static_cast<int>(cities[i].x), static_cast<int>(cities[i].y), CITY_RADIUS, color);
        char label[16];
        snprintf(label, sizeof(label), "%d", i);
        DrawText(label, static_cast<int>(cities[i].x) + 10, static_cast<int>(cities[i].y) - 5, 14, DARKGRAY);
    }

    // Draw the route
    for (int i = 0; i < num_cities - 1; ++i) {
        int idx1 = route[i];
        int idx2 = route[i + 1];
        DrawLine(static_cast<int>(cities[idx1].x), static_cast<int>(cities[idx1].y),
                 static_cast<int>(cities[idx2].x), static_cast<int>(cities[idx2].y), DARKGRAY);
    }
    int last = route[num_cities - 1];
    int first = route[0];
    DrawLine(static_cast<int>(cities[last].x), static_cast<int>(cities[last].y),
             static_cast<int>(cities[first].x), static_cast<int>(cities[first].y), DARKGRAY);

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
