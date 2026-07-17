// include/visualization/renderer.h
#pragma once

#include "include/tsp/city.h"

namespace tsp::visualization {

void init_window(int num_cities);
void draw_route(const City* cities, int num_cities, const int* route, int generation, int max_generations);
void wait_for_close();
void close_window();

} // namespace tsp::visualization