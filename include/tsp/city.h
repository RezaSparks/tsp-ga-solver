// include/tsp/city.h
//
// Represents a single city as an (x, y) coordinate on the map.
// NOTE: Coordinates are double to support TSPLIB EUC_2D format (Issue #7).
#pragma once

#include <random>
#include <string>
#include <vector>

namespace tsp {

    struct City {
        double x = 0.0;
        double y = 0.0;
    };

    // Reads `n` city coordinates interactively from stdin.
    void input_cities(City* cities, int n);

    // Fills `cities` with `n` random coordinates using the provided RNG.
    void init_cities_random(City* cities, int n, std::mt19937& rng);

    // Euclidean distance between two cities.
    double distance(const City* a, const City* b);

    // Load cities from a CSV file: header "x,y" then one city per line.
    // Returns true on success, false on failure (logs to stderr).
    bool load_cities_from_csv(const std::string& filepath, std::vector<City>& out);

} // namespace tsp
