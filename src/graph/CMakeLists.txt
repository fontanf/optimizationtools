add_library(OptimizationTools_graph)
target_sources(OptimizationTools_graph PRIVATE
    adjacency_list_graph.cpp
    clique_graph.cpp)
target_include_directories(OptimizationTools_graph PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
add_library(OptimizationTools::graph ALIAS OptimizationTools_graph)
