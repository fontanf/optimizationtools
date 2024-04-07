enable_testing()

add_executable(
  optimizationtools_test
  containers/space_efficient_array_test.cpp
)
target_link_libraries(optimizationtools_test optimizationtools)
target_link_libraries(optimizationtools_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(optimizationtools_test)
