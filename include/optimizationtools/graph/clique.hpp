#include "optimizationtools/graph/adjacency_list_graph.hpp"

namespace optimizationtools
{

/*
 * Edge clique cover: a set of cliques such that every edge of the graph belongs
 * to at least one clique.
 *
 * Vertex clique cover: a set of cliques such that every edge of the graph
 * belongs to at least one clique.
 *
 * Edge clique partition: a set of cliques such that every edge of the graph
 * belongs to exactly one clique.
 *
 * Vertex clique partition: a set of cliques such that every vertex of the graph
 * belongs to exactly one clique.
 */

/**
 * Compute an edge clique cover of the graph.
 *
 * For each edge, a clique is built by greedily adding the candidate vertex with
 * the highest weight.
 *
 * Duplicate cliques are removed.
 */
std::vector<std::vector<VertexId>> edge_clique_cover(
        const AdjacencyListGraph& graph);

/**
 * Compute an vertex clique cover of the graph.
 *
 * For each vertex, a clique is built by greedily adding the candidate vertex
 * with the highest weight.
 *
 * Duplicate cliques are removed.
 */
std::vector<std::vector<VertexId>> vertex_clique_cover(
        const AdjacencyListGraph& graph);

/**
 * Compute an edge clique partition of the graph.
 *
 * While there remains an unselected edge, the algorithm selects the unselected
 * edge with the highest weight and greedily builds a clique from this edge
 * using only unselected edges.
 */
std::vector<std::vector<VertexId>> edge_clique_partition(
        const AdjacencyListGraph& graph);

/**
 * Compute a vertex clique partition of the graph.
 *
 * For each vertex, it is added to the maximum-weight compatible clique. If
 * there is no compatible clique, a new clique contaiing only this vertex is
 * added.
 */
std::vector<std::vector<VertexId>> vertex_clique_partition_1(
        const AdjacencyListGraph& graph);

/**
 * Compute a vertex clique partition of the graph.
 *
 * While there remains unselected vertices:
 * - For each edge, build a clique by greedily adding the unselected vertex with
 *   the highest weight
 * - Select the best clique
 *
 * This algorithm guarantees to return a clique which is at least as good as the
 * best clique of size 3.
 */
std::vector<std::vector<VertexId>> vertex_clique_partition_2(
        const AdjacencyListGraph& graph);

}
