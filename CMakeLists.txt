cmake_minimum_required(VERSION 3.16)

project(tsp_ga_solver
    VERSION 0.1.0
    DESCRIPTION "A configurable genetic-algorithm solver for the TSP"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG        v3.2.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cxxopts)

# === GoogleTest: Issue #4 ===
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(googletest)

add_library(tsp_core STATIC
    src/city.cpp
    src/population.cpp
)
target_include_directories(tsp_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(tsp_visualization STATIC
    src/renderer.cpp
)
target_include_directories(tsp_visualization PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(tsp_visualization PUBLIC raylib)

add_library(tsp_cli STATIC
    src/cli.cpp
)
target_include_directories(tsp_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(tsp_cli PUBLIC cxxopts::cxxopts)

add_executable(tsp_solver src/main.cpp)
target_link_libraries(tsp_solver PRIVATE tsp_core tsp_visualization tsp_cli)

if(UNIX AND NOT APPLE)
    target_link_libraries(tsp_visualization PUBLIC m pthread dl rt X11)
endif()

# === Tests: Issue #4 ===
option(TSP_BUILD_TESTS "Build unit tests" OFF)
if(TSP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "tsp-ga-solver configuration:")
message(STATUS "  Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard:    ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests:     ${TSP_BUILD_TESTS}")