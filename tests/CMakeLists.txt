# tests/CMakeLists.txt — Issue #4: GoogleTest integration

add_executable(tsp_tests
    test_core.cpp
)

target_link_libraries(tsp_tests PRIVATE
    tsp_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tsp_tests)