cmake_minimum_required(VERSION 3.16)

project(tsp_ga_solver
    VERSION 0.1.0
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
# Dependencies
# ---------------------------------------------------------------------------
# Raylib is fetched and built from source via CMake's FetchContent, so the
# project builds the same way on Linux, macOS, and Windows without relying
# on a system package manager or a vendored copy checked into the repo.
include(FetchContent)

# Keep raylib's own examples/games out of our build.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# cxxopts: header-only CLI argument parser (Issue #4). Keep its own
# examples/tests out of our build.
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG        v3.2.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cxxopts)

# ---------------------------------------------------------------------------
# Core library: the TSP + GA logic, kept separate from main() so that
# /tests can link against it directly without pulling in an executable.
# ---------------------------------------------------------------------------
add_library(tsp_core STATIC
    src/city.cpp
    src/population.cpp
)
target_include_directories(tsp_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---------------------------------------------------------------------------
# Visualization library: isolates the Raylib dependency so a future
# headless build can drop this target entirely (Milestone 1, next task).
# ---------------------------------------------------------------------------
add_library(tsp_visualization STATIC
    src/renderer.cpp
)
target_include_directories(tsp_visualization PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(tsp_visualization PUBLIC raylib)

# ---------------------------------------------------------------------------
# CLI library: argument parsing (Issue #4), isolated so it can be extended
# for Issue #5 (headless flag) and reused by /tests without pulling in
# raylib or main().
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

# raylib on Linux needs a few system libs when built from source; on
# macOS/Windows these are no-ops or handled by raylib's own CMake config.
if(UNIX AND NOT APPLE)
    target_link_libraries(tsp_visualization PUBLIC m pthread dl rt X11)
endif()

# ---------------------------------------------------------------------------
# Tests (wired up in a later milestone -- kept here so `add_subdirectory`
# doesn't need to change when tests are added).
# ---------------------------------------------------------------------------
option(TSP_BUILD_TESTS "Build unit tests" OFF)
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
