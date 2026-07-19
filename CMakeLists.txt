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
# Dependencies: raylib, cxxopts, (optionally) GoogleTest
# ---------------------------------------------------------------------------
# Two ways to satisfy these, and both are supported here -- pick whichever
# is available on the machine doing the build:
#
#   1. vcpkg (fast: no internet needed at build time once packages are
#      installed locally). To use it, pass the toolchain file on the
#      cmake COMMAND LINE:
#          cmake -B build -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
#      This is the only correct way to specify it: a toolchain file only
#      takes effect if CMake knows about it before the first project()/
#      enable_language() call above -- setting CMAKE_TOOLCHAIN_FILE via
#      set() *inside* this file, after project() has already run, is too
#      late and silently does nothing. It must also never be hardcoded to
#      one person's local path (e.g. "C:/dev/vcpkg/..."), since that
#      breaks the build for every other machine, including CI -- which is
#      exactly what happened when that was tried here.
#
#   2. FetchContent (default fallback): downloads and builds each
#      dependency from source. Slower and needs internet, but works out
#      of the box on any machine, which is what CI and new contributors
#      without vcpkg set up rely on.
#
# find_package(... CONFIG QUIET) detects vcpkg-provided packages without
# erroring out if they're absent, so the FetchContent fallback below only
# runs when nothing already satisfied the dependency.
include(FetchContent)

# ---------------------------------------------------------------------------
# Diagnostics: make it obvious *why* find_package will or won't see vcpkg,
# instead of silently sliding into a slow FetchContent download when it
# doesn't. Print this unconditionally (not just on failure) so it shows up
# on every configure and is easy to compare against what you expect.
# ---------------------------------------------------------------------------
if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
else()
    message(STATUS "CMAKE_TOOLCHAIN_FILE is not set -- vcpkg packages will not be found; "
                    "FetchContent will be used for every dependency below.")
endif()
if(DEFINED VCPKG_TARGET_TRIPLET)
    message(STATUS "VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")
endif()
# VCPKG_TOOLCHAIN is the variable vcpkg's own toolchain script sets once it
# has actually run (it's how the script guards against re-running itself).
# It's a much more reliable signal than checking for any specific installed
# package, since it tells us definitively whether the toolchain executed
# at all, independent of which packages happen to be installed.
if(DEFINED VCPKG_TOOLCHAIN)
    message(STATUS "vcpkg toolchain ran  = ${VCPKG_TOOLCHAIN}")
endif()
# If CMAKE_TOOLCHAIN_FILE was set but VCPKG_TOOLCHAIN never got defined,
# the toolchain file was never actually processed -- almost always because
# this build/ directory was configured once before *without* it, and CMake
# is silently reusing that stale cache. The fix is:
#     rm -rf build   (or: rmdir /s /q build   on Windows)
# then reconfigure from scratch with the toolchain flag. A toolchain file
# is a one-time, first-configure-only setting -- passing it again on an
# already-configured build directory has no effect. (This project's
# CMakePresets.json sidesteps this by giving vcpkg and non-vcpkg builds
# separate binaryDir locations, so their caches can never collide.)
if(CMAKE_TOOLCHAIN_FILE AND NOT DEFINED VCPKG_TOOLCHAIN)
    message(WARNING
        "CMAKE_TOOLCHAIN_FILE is set but vcpkg does not appear to have run. "
        "This almost always means the build directory was configured once "
        "before without the toolchain file, and CMake is reusing that "
        "stale cache. Delete the build directory entirely and reconfigure "
        "from scratch, e.g.: rm -rf build && cmake -B build "
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif()

find_package(raylib CONFIG QUIET)
if(raylib_FOUND)
    message(STATUS "raylib: found via find_package (vcpkg or system install)")
else()
    message(STATUS "raylib: not found via find_package -- fetching from source instead")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

find_package(cxxopts CONFIG QUIET)
if(cxxopts_FOUND)
    message(STATUS "cxxopts: found via find_package (vcpkg or system install)")
else()
    message(STATUS "cxxopts: not found via find_package -- fetching from source instead")
    set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG        v3.2.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(cxxopts)
endif()

# GoogleTest: unit testing framework. Only fetched/found when tests are
# actually requested, same reasoning as before (don't pay for it on every
# normal build).
option(TSP_BUILD_TESTS "Build unit tests" OFF)

if(TSP_BUILD_TESTS)
    find_package(GTest CONFIG QUIET)
    if(GTest_FOUND)
        message(STATUS "GoogleTest: found via find_package (vcpkg or system install)")
    else()
        message(STATUS "GoogleTest: not found via find_package -- fetching from source instead")
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(googletest)
    endif()
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