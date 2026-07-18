cmake_minimum_required(VERSION 3.16)

project(tsp_ga_solver
    VERSION 0.2.0
    DESCRIPTION "A configurable genetic-algorithm solver for the TSP, with optional Raylib visualization"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Global project settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable Microsoft's deprecated function warnings (scanf, fopen, etc.)
if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Always emit compile_commands.json -- useful for clangd / IDE tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to a Release build if the user didn't pick one (single-config
# generators like Makefiles/Ninja only -- multi-config generators such as
# Visual Studio or Xcode ignore CMAKE_BUILD_TYPE and use --config instead).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# vcpkg toolchain
# ---------------------------------------------------------------------------
# Use vcpkg for package management. This finds dependencies installed via
# vcpkg instead of downloading them from the internet.
set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(raylib CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

# GoogleTest: unit testing framework. Only required when building tests.
option(TSP_BUILD_TESTS "Build unit tests" OFF)

if(TSP_BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
endif()

# ---------------------------------------------------------------------------
# Core library: the TSP + GA logic + TSPLIB parser
# ---------------------------------------------------------------------------
add_library(tsp_core STATIC
    src/city.cpp
    src/population.cpp
    src/tsplib_parser.cpp
)
target_include_directories(tsp_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---------------------------------------------------------------------------
# Visualization library: isolates the Raylib dependency
# ---------------------------------------------------------------------------
add_library(tsp_visualization STATIC
    src/renderer.cpp
)
target_include_directories(tsp_visualization PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(tsp_visualization PUBLIC raylib)

# ---------------------------------------------------------------------------
# CLI library: argument parsing
# ---------------------------------------------------------------------------
add_library(tsp_cli STATIC
    src/cli.cpp
)
target_include_directories(tsp_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(tsp_cli PUBLIC cxxopts::cxxopts)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(tsp_solver src/main.cpp)
target_link_libraries(tsp_solver PRIVATE tsp_core tsp_visualization tsp_cli)

# raylib on Linux needs a few system libs when built from source
if(UNIX AND NOT APPLE)
    target_link_libraries(tsp_visualization PUBLIC m pthread dl rt X11)
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(TSP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------
# Helpful summary printed at configure time
# ---------------------------------------------------------------------------
message(STATUS "tsp-ga-solver configuration:")
message(STATUS "  Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard:    ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests:     ${TSP_BUILD_TESTS}")