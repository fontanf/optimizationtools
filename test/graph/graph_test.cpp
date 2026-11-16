#include "optimizationtools/graph/adjacency_list_graph.hpp"
#include "optimizationtools/graph/adjacency_matrix_graph.hpp"
#include "optimizationtools/graph/clique_graph.hpp"

#include <gtest/gtest.h>

#include <algorithm>

using namespace optimizationtools;

namespace
{

/**
 * Test graph:
 * - clique {0, 1, 2}
 * - clique {2, 3}
 * - vertex 4 isolated
 * Weights: 1, 2, 3, 4, 5.
 */
const std::vector<std::vector<VertexId>> cliques = {{0, 1, 2}, {2, 3}};
const std::vector<Weight> weights = {1, 2, 3, 4, 5};
const std::vector<std::vector<VertexId>> expected_neighbors = {
    {1, 2}, {0, 2}, {0, 1, 3}, {2}, {}};

AdjacencyListGraph build_adjacency_list_graph()
{
    AdjacencyListGraphBuilder graph_builder;
    for (Weight weight: weights)
        graph_builder.add_vertex(weight);
    for (const auto& clique: cliques)
        for (VertexPos pos_1 = 0; pos_1 < (VertexPos)clique.size(); ++pos_1)
            for (VertexPos pos_2 = pos_1 + 1; pos_2 < (VertexPos)clique.size(); ++pos_2)
                graph_builder.add_edge(clique[pos_1], clique[pos_2]);
    return graph_builder.build();
}

AdjacencyMatrixGraph build_adjacency_matrix_graph()
{
    AdjacencyMatrixGraphBuilder graph_builder;
    for (Weight weight: weights)
        graph_builder.add_vertex(weight);
    for (const auto& clique: cliques)
        for (VertexPos pos_1 = 0; pos_1 < (VertexPos)clique.size(); ++pos_1)
            for (VertexPos pos_2 = pos_1 + 1; pos_2 < (VertexPos)clique.size(); ++pos_2)
                graph_builder.add_edge(clique[pos_1], clique[pos_2]);
    return graph_builder.build();
}

CliqueGraph build_clique_graph()
{
    CliqueGraphBuilder graph_builder;
    for (Weight weight: weights)
        graph_builder.add_vertex(weight);
    for (const auto& clique: cliques)
        graph_builder.add_clique(clique);
    return graph_builder.build();
}

template <typename Graph>
std::vector<VertexId> sorted_neighbors(
        const Graph& graph,
        VertexId vertex_id)
{
    std::vector<VertexId> neighbors(
            graph.neighbors_begin(vertex_id),
            graph.neighbors_end(vertex_id));
    std::sort(neighbors.begin(), neighbors.end());
    return neighbors;
}

template <typename Graph>
void check_graph(const Graph& graph)
{
    EXPECT_EQ(graph.number_of_vertices(), 5);
    EXPECT_EQ(graph.number_of_edges(), 4);
    EXPECT_EQ(graph.highest_degree(), 3);
    EXPECT_EQ(graph.total_weight(), 15);
    EXPECT_DOUBLE_EQ(graph.density(), 8.0 / 25);
    EXPECT_DOUBLE_EQ(graph.average_degree(), 8.0 / 5);
    EXPECT_DOUBLE_EQ(density(graph), 8.0 / 25);
    EXPECT_NO_THROW(graph.check_vertex_index(4));
    EXPECT_THROW(graph.check_vertex_index(5), std::out_of_range);
    EXPECT_THROW(graph.check_vertex_index(-1), std::out_of_range);
    for (VertexId vertex_id = 0; vertex_id < 5; ++vertex_id) {
        EXPECT_EQ(graph.weight(vertex_id), weights[vertex_id]);
        EXPECT_EQ(graph.degree(vertex_id), (VertexPos)expected_neighbors[vertex_id].size());
        EXPECT_EQ(sorted_neighbors(graph, vertex_id), expected_neighbors[vertex_id]);
    }
}

}

TEST(Graph, AdjacencyListGraph)
{
    check_graph(build_adjacency_list_graph());
}

TEST(Graph, AdjacencyMatrixGraph)
{
    check_graph(build_adjacency_matrix_graph());
}

TEST(Graph, CliqueGraph)
{
    check_graph(build_clique_graph());
}

TEST(Graph, AdjacencyListGraphFromAdjacencyMatrixGraph)
{
    check_graph(AdjacencyListGraph(build_adjacency_matrix_graph()));
}

TEST(Graph, AdjacencyListGraphFromCliqueGraph)
{
    check_graph(AdjacencyListGraph(build_clique_graph()));
}

TEST(Graph, Complementary)
{
    AdjacencyListGraph graph = build_adjacency_list_graph().complementary();
    EXPECT_EQ(graph.number_of_vertices(), 5);
    EXPECT_EQ(graph.number_of_edges(), 10 - 4);
    EXPECT_EQ(graph.total_weight(), 15);
    EXPECT_EQ(sorted_neighbors(graph, 4), std::vector<VertexId>({0, 1, 2, 3}));
    EXPECT_EQ(sorted_neighbors(graph, 2), std::vector<VertexId>({4}));
}
