// src/renderer.cpp – DEBUG VERSION: raw coordinates, red background
#include "include/visualization/renderer.h"

#include <raylib.h>
#include <cstdio>
#include <cmath>

namespace tsp::visualization {

static const int SCREEN_WIDTH = 900;
static const int SCREEN_HEIGHT = 700;
static bool window_ready = false;

void init_window(int /*num_cities*/) {
    // Enable debug logging to see what Raylib is doing
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TSP GA Solver (DEBUG)");
    SetTargetFPS(60);
    window_ready = true;
    printf("[RENDERER] Window initialized.\n");
}

void draw_route(const City* cities, int num_cities, const int* route,
                int generation, int max_generations) {
    if (!window_ready) {
        printf("[RENDERER] ERROR: window not ready.\n");
        return;
    }
    if (!cities || num_cities <= 0 || !route) {
        printf("[RENDERER] ERROR: invalid input (cities=%p, num=%d, route=%p)\n",
               (void*)cities, num_cities, (void*)route);
        return;
    }

    // Process window events (keeps the UI responsive)
    PollInputEvents();

    // Start drawing – clear with RED so we can see if it works
    BeginDrawing();
    ClearBackground(RED);

    // Draw a BIG BLUE CIRCLE at the centre – this will confirm drawing works
    DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, BLUE);
    DrawText("If you see this text and a blue circle, drawing works!", 100, 50, 20, WHITE);

    // Draw generation info
    char buf[64];
    snprintf(buf, sizeof(buf), "Generation: %d / %d", generation + 1, max_generations);
    DrawText(buf, 20, 20, 20, YELLOW);

    // Draw route using RAW coordinates (no scaling)
    if (num_cities > 1) {
        // Print first few coordinates to console for debugging
        static int call_count = 0;
        if (call_count++ % 10 == 0) {
            printf("[RENDERER] Drawing %d cities. Sample: city 0 = (%.2f, %.2f)\n",
                   num_cities, cities[0].x, cities[0].y);
        }

        for (int i = 0; i < num_cities - 1; ++i) {
            int a = route[i];
            int b = route[i + 1];
            // Ensure indices are valid
            if (a < 0 || a >= num_cities || b < 0 || b >= num_cities) {
                printf("[RENDERER] WARNING: invalid route index at %d: %d->%d\n", i, a, b);
                continue;
            }
            DrawLine(
                static_cast<int>(cities[a].x),
                static_cast<int>(cities[a].y),
                static_cast<int>(cities[b].x),
                static_cast<int>(cities[b].y),
                GREEN
            );
        }
        // closing edge
        int last = route[num_cities - 1];
        int first = route[0];
        DrawLine(
            static_cast<int>(cities[last].x),
            static_cast<int>(cities[last].y),
            static_cast<int>(cities[first].x),
            static_cast<int>(cities[first].y),
            GREEN
        );

        // Draw city circles
        for (int i = 0; i < num_cities; ++i) {
            Color col = (i == route[0]) ? RED : BLUE;
            DrawCircle(
                static_cast<int>(cities[i].x),
                static_cast<int>(cities[i].y),
                6, col
            );
            char label[16];
            snprintf(label, sizeof(label), "%d", i);
            DrawText(label,
                     static_cast<int>(cities[i].x) + 10,
                     static_cast<int>(cities[i].y) - 6,
                     14, WHITE);
        }
    }

    EndDrawing();

    // Yield to OS to keep window responsive
    WaitTime(0.001);
}

bool should_close() {
    return WindowShouldClose();
}

void wait_for_close() {
    while (!WindowShouldClose()) {
        PollInputEvents();
        WaitTime(0.01);
    }
}

void close_window() {
    CloseWindow();
    window_ready = false;
}

// The Viewport function is not needed for this debug version
Viewport compute_viewport(const City*, int) {
    return {0.0, 0.0, 1.0, 20};
}

} // namespace tsp::visualization