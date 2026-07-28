cmake_minimum_required(VERSION 3.16)
project(tsp_ga_solver VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TSP_BUILD_TESTS "Build unit tests" OFF)

# Try find_package first (vcpkg), fallback to FetchContent
find_package(cxxopts QUIET)
if(NOT cxxopts_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG        v3.1.1
    )
    FetchContent_MakeAvailable(cxxopts)
endif()

if(TSP_BUILD_TESTS)
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

add_library(tsp_core
    src/city.cpp
    src/tsplib_parser.cpp
    src/population.cpp
)

target_include_directories(tsp_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tsp_core
    PUBLIC
        cxxopts::cxxopts
)

add_library(tsp_cli
    src/cli.cpp
)

target_include_directories(tsp_cli
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tsp_cli
    PUBLIC
        cxxopts::cxxopts
        tsp_core
)

add_executable(tsp_solver
    src/main.cpp
)

target_include_directories(tsp_solver
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tsp_solver
    PRIVATE
        tsp_core
        tsp_cli
)

if(TSP_BUILD_TESTS)
    enable_testing()

    add_executable(tsp_tests
        tests/test_core.cpp
    )

    target_include_directories(tsp_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(tsp_tests
        PRIVATE
            tsp_core
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(tsp_tests)
endif()

install(TARGETS tsp_solver
    RUNTIME DESTINATION bin
)