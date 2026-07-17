// include/tsp/city.h
//
// Represents a single city as an (x, y) coordinate on the map, plus the
// free functions used to populate a city list and measure distances.
//
// NOTE: This is a direct, minimally-modified port of the original C-style
// code from the Visual Studio project. It intentionally keeps the plain
// struct + free-function shape for now -- modernization (RAII, classes,
// std::vector, etc.) is scoped to a later milestone so this change stays
// focused on "get the project building with CMake."
#pragma once

namespace tsp {

    struct City {
        int x = 0;
        int y = 0;
    };

    // Reads `n` city coordinates interactively from stdin. Accepts either
    // "x y" or "(x, y)" formatted input per city.
    void input_cities(City* cities, int n);

    // Fills `cities` with `n` random coordinates in an 800x600-ish window.
    // Useful for non-interactive / headless runs (added in a later milestone).
    void init_cities_random(City* cities, int n);

    // Euclidean distance between two cities.
    double distance(const City* a, const City* b);

} // namespace tsp
