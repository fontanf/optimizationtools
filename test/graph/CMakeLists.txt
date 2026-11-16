add_executable(OptimizationTools_graph_test)
target_sources(OptimizationTools_graph_test PRIVATE
    graph_test.cpp)
target_link_libraries(OptimizationTools_graph_test
    OptimizationTools_graph
    GTest::gtest_main)
gtest_discover_tests(OptimizationTools_graph_test)
