// include/visualization/renderer.h
//
// Thin Raylib-based drawing helper. Kept separate from the GA/city logic
// so that a future headless build (Milestone 1, "non-visualization mode")
// can compile this file out entirely without touching the solver code.
#pragma once

#include "include/tsp/city.h"

namespace tsp::visualization {

	// Draws the tour described by `route` over `cities` (as connected line
	// segments + city markers), plus a "Generation: N" label. Must be called
	// between Raylib's BeginDrawing()/EndDrawing().
	void draw_route(const tsp::City* cities, const int* route, int n, int generation);

} // namespace tsp::visualization