// src/tsplib_parser.cpp
#include "include/tsp/tsplib_parser.h"

#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace tsp {

    static std::string trim(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
        return s.substr(start, end - start);
    }

    static std::string to_upper(const std::string& s) {
        std::string result = s;
        for (char& c : result) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        return result;
    }

    bool load_tsplib(const std::string& filepath, std::vector<City>& out) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::fprintf(stderr, "Error: could not open TSPLIB file: %s\n", filepath.c_str());
            return false;
        }

        out.clear();

        std::string line;
        bool in_coord_section = false;
        std::string edge_weight_type;
        int expected_dimension = -1;
        int cities_read = 0;

        while (std::getline(file, line)) {
            std::string trimmed = trim(line);
            if (trimmed.empty()) continue;

            std::string upper = to_upper(trimmed);

            // Check for NODE_COORD_SECTION start
            if (upper == "NODE_COORD_SECTION") {
                in_coord_section = true;
                continue;
            }

            // Check for EOF marker
            if (upper == "EOF") {
                break;
            }

            if (!in_coord_section) {
                // Parse specification section
                size_t colon_pos = trimmed.find(':');
                if (colon_pos != std::string::npos) {
                    std::string key = trim(trimmed.substr(0, colon_pos));
                    std::string value = trim(trimmed.substr(colon_pos + 1));
                    std::string key_upper = to_upper(key);

                    if (key_upper == "EDGE_WEIGHT_TYPE") {
                        edge_weight_type = to_upper(value);
                    } else if (key_upper == "DIMENSION") {
                        try {
                            expected_dimension = std::stoi(value);
                        } catch (...) {
                            std::fprintf(stderr, "Error: invalid DIMENSION value: %s\n", value.c_str());
                            return false;
                        }
                    }
                }
                continue;
            }

            // In NODE_COORD_SECTION: parse "index x y"
            std::stringstream ss(trimmed);
            int index;
            double x, y;
            if (!(ss >> index >> x >> y)) {
                std::fprintf(stderr, "Error: malformed coordinate line: %s\n", trimmed.c_str());
                return false;
            }

            City c;
            c.x = x;
            c.y = y;
            out.push_back(c);
            cities_read++;
        }

        // Validation
        if (edge_weight_type.empty()) {
            std::fprintf(stderr, "Error: EDGE_WEIGHT_TYPE not specified in %s\n", filepath.c_str());
            return false;
        }

        if (edge_weight_type != "EUC_2D") {
            std::fprintf(stderr,
                "Error: unsupported EDGE_WEIGHT_TYPE '%s'. "
                "Only EUC_2D is supported.\n",
                edge_weight_type.c_str());
            std::fprintf(stderr,
                "To request support for other formats, please open an issue: "
                "https://github.com/RezaSparks/tsp-ga-solver/issues\n");
            return false;
        }

        if (expected_dimension > 0 && cities_read != expected_dimension) {
            std::fprintf(stderr,
                "Error: expected %d cities but read %d from %s\n",
                expected_dimension, cities_read, filepath.c_str());
            return false;
        }

        if (out.size() < 3) {
            std::fprintf(stderr, "Error: TSPLIB file must contain at least 3 cities (got %zu)\n",
                         out.size());
            return false;
        }

        return true;
    }

} // namespace tsp
