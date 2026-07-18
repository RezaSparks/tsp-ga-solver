// src/city.cpp
#include "include/tsp/city.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

namespace tsp {

    void input_cities(City* cities, int n) {
        for (int i = 0; i < n; i++) {
            std::printf("Enter coordinates for city %d (x y): ", i + 1);
            double x = 0.0, y = 0.0;
            if (std::scanf(" (%lf , %lf)", &x, &y) != 2) {
                std::scanf("%lf %lf", &x, &y);
            }
            cities[i].x = x;
            cities[i].y = y;
        }
    }

    void init_cities_random(City* cities, int n, std::mt19937& rng) {
        std::uniform_int_distribution<int> x_dist(50, 850);
        std::uniform_int_distribution<int> y_dist(50, 650);
        for (int i = 0; i < n; i++) {
            cities[i].x = static_cast<double>(x_dist(rng));
            cities[i].y = static_cast<double>(y_dist(rng));
        }
    }

    double distance(const City* a, const City* b) {
        const double dx = a->x - b->x;
        const double dy = a->y - b->y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool load_cities_from_csv(const std::string& filepath, std::vector<City>& out) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::fprintf(stderr, "Error: could not open CSV file: %s\n", filepath.c_str());
            return false;
        }

        out.clear();
        std::string line;
        bool first_line = true;
        int line_num = 0;

        while (std::getline(file, line)) {
            line_num++;
            // Skip empty lines
            if (line.empty()) continue;

            // Skip header line (contains non-numeric text like "x,y")
            if (first_line) {
                first_line = false;
                // Check if this line looks like a header (contains letters)
                bool has_letters = false;
                for (char c : line) {
                    if (std::isalpha(static_cast<unsigned char>(c))) {
                        has_letters = true;
                        break;
                    }
                }
                if (has_letters) continue;  // skip header
            }

            std::stringstream ss(line);
            std::string x_str, y_str;
            if (!std::getline(ss, x_str, ',') || !std::getline(ss, y_str, ',')) {
                std::fprintf(stderr, "Error: malformed line %d in %s: %s\n",
                             line_num, filepath.c_str(), line.c_str());
                return false;
            }

            try {
                City c;
                c.x = std::stod(x_str);
                c.y = std::stod(y_str);
                out.push_back(c);
            } catch (const std::exception&) {
                std::fprintf(stderr, "Error: invalid number on line %d in %s: %s\n",
                             line_num, filepath.c_str(), line.c_str());
                return false;
            }
        }

        if (out.size() < 3) {
            std::fprintf(stderr, "Error: CSV file must contain at least 3 cities (got %zu)\n",
                         out.size());
            return false;
        }

        return true;
    }

} // namespace tsp
