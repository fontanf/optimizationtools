# Add "optimizationtools" libray.
add_library(
    optimizationtools
    ../include/optimizationtools/containers/indexed_set.hpp
    ../include/optimizationtools/containers/indexed_map.hpp
    ../include/optimizationtools/containers/doubly_indexed_map.hpp
    ../include/optimizationtools/containers/indexed_binary_heap.hpp
    ../include/optimizationtools/containers/sorted_on_demand_array.hpp
    ../include/optimizationtools/containers/partial_set.hpp
    ../include/optimizationtools/graph/abstract_graph.hpp
    ../include/optimizationtools/graph/adjacency_list_graph.hpp
    graph/adjacency_list_graph.cpp
    ../include/optimizationtools/graph/adjacency_matrix_graph.hpp
    ../include/optimizationtools/graph/clique_graph.hpp
    graph/clique_graph.cpp
    ../include/optimizationtools/utils/utils.hpp
    ../include/optimizationtools/utils/timer.hpp
    utils/timer.cpp
    ../include/optimizationtools/utils/compose_stream.hpp
    utils/compose_stream.cpp
    ../include/optimizationtools/utils/logger.hpp
    utils/logger.cpp
    utils/parameters.cpp
    ../include/optimizationtools/utils/parameters.hpp
    utils/output.cpp
    ../include/optimizationtools/utils/output.hpp)

target_include_directories(optimizationtools PUBLIC ../include)

target_link_libraries(optimizationtools PRIVATE nlohmann_json::nlohmann_json)
