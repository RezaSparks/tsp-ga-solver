// include/tsp/tsplib_parser.h
//
// TSPLIB instance parser — supports EUC_2D EDGE_WEIGHT_TYPE only.
// All other formats are rejected with a clear error message (Issue #9).
#pragma once

#include <string>
#include <vector>

#include "include/tsp/city.h"

namespace tsp {

    // Parse a TSPLIB .tsp file. Only EUC_2D coordinates are supported.
    // Returns true on success, false on failure (logs to stderr).
    bool load_tsplib(const std::string& filepath, std::vector<City>& out);

} // namespace tsp
