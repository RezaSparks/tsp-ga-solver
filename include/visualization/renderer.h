// include/visualization/renderer.h
//
// Thin Raylib-based drawing helper.
// Issue: TSPLIB instances have varying coordinate ranges; we scale to fit window.
#pragma once

#include "include/tsp/city.h"

namespace tsp::visualization {

    struct Viewport {
        double min_x = 0.0;
        double min_y = 0.0;
        double scale = 1.0;
        int padding = 20;
    };

    void init_window(int num_cities);
    void draw_route(const City* cities, int num_cities, const int* route,
                    int generation, int max_generations);
    bool should_close();
    void wait_for_close();
    void close_window();

    // Compute scaling factors so all cities fit in the window with padding.
    // Call once before the first draw, or when city set changes.
    Viewport compute_viewport(const City* cities, int num_cities);

} // namespace tsp::visualization
