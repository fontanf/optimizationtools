add_library(optimizationtools_graph)
target_sources(optimizationtools_graph PRIVATE
    adjacency_list_graph.cpp
    clique_graph.cpp)
target_include_directories(optimizationtools_graph PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
add_library(OptimizationTools::graph ALIAS optimizationtools_graph)
