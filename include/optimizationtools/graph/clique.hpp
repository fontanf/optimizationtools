#include "optimizationtools/graph/adjacency_list_graph.hpp"

namespace optimizationtools
{

/**
 * Compute a clique cover of the graph.
 *
 * For each edge, a clique is built by greedily adding the candidate vertex with
 * the highest weight.
 *
 * Duplicate cliques are removed.
 */
std::vector<std::vector<VertexId>> clique_cover(
        const AdjacencyListGraph& graph);

/**
 * Compute a clique partition of the graph.
 *
 * For each vertex, it is added to the maximum-weight compatible clique. If
 * there is no compatible clique, a new clique contaiing only this vertex is
 * added.
 */
std::vector<std::vector<VertexId>> clique_partition_1(
        const AdjacencyListGraph& graph);

/**
 * Compute a clique partition of the graph.
 *
 * While there remains unselected vertices:
 * - For each edge, build a clique by greedily adding the unselected vertex with
 *   the highest weight
 * - Select the best clique
 *
 * This algorithm guarantees to return a clique which is at least as good as the
 * best clique of size 3.
 */
std::vector<std::vector<VertexId>> clique_partition_2(
        const AdjacencyListGraph& graph);

}
