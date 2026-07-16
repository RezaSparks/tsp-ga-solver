#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 

// renderer.cpp
#include "visualization/renderer.h"
#include <raylib.h>

void draw_route(const city_t* cities, const int* route, int n, int generation) {
    // Draw lines
    for (int i = 0; i < n - 1; i++) {
        int a = route[i], b = route[i + 1];
        DrawLine(cities[a].x, cities[a].y, cities[b].x, cities[b].y, GREEN);
    }
    DrawLine(cities[route[n - 1]].x, cities[route[n - 1]].y, cities[route[0]].x, cities[route[0]].y, GREEN);

    // Draw cities
    for (int i = 0; i < n; i++) {
        DrawCircle(cities[i].x, cities[i].y, 5, RED);
    }

    char text[100];
    sprintf(text, "Generation: %d", generation);
    DrawText(text, 10, 10, 20, WHITE);
}