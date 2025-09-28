#include "optimizationtools/graph/adjacency_list_graph.hpp"

namespace optimizationtools
{

std::vector<uint8_t> bipartite_graph_identify(
        const AdjacencyListGraph& graph);

std::vector<uint8_t> bipartite_graph_maximum_matching(
        const AdjacencyListGraph& graph);

std::vector<uint8_t> bipartite_graph_minimum_cover(
        const AdjacencyListGraph& graph);

}
