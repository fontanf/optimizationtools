#include "optimizationtools/graph/adjacency_list_graph.hpp"
#include "optimizationtools/graph/adjacency_matrix_graph.hpp"
#include "optimizationtools/graph/clique_graph.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <thread>

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

TEST(Graph, AdjacencyMatrixGraphNestedIteration)
{
    // Iterate over the neighbors of the neighbors of each vertex while
    // iterating over its neighbors.
    AdjacencyMatrixGraph graph = build_adjacency_matrix_graph();
    for (VertexId vertex_id = 0; vertex_id < graph.number_of_vertices(); ++vertex_id) {
        std::vector<VertexId> neighbors;
        for (auto it = graph.neighbors_begin(vertex_id);
                it != graph.neighbors_end(vertex_id);
                ++it) {
            neighbors.push_back(*it);
            EXPECT_EQ(sorted_neighbors(graph, *it), expected_neighbors[*it]);
        }
        EXPECT_EQ(neighbors, expected_neighbors[vertex_id]);
    }
}

TEST(Graph, AdjacencyMatrixGraphConcurrentIteration)
{
    // Random graph large enough for the threads to overlap.
    VertexId number_of_vertices = 300;
    AdjacencyMatrixGraphBuilder graph_builder;
    for (VertexId vertex_id = 0; vertex_id < number_of_vertices; ++vertex_id)
        graph_builder.add_vertex();
    std::vector<std::vector<VertexId>> expected(number_of_vertices);
    for (VertexId vertex_id_1 = 0; vertex_id_1 < number_of_vertices; ++vertex_id_1) {
        for (VertexId vertex_id_2 = vertex_id_1 + 1; vertex_id_2 < number_of_vertices; ++vertex_id_2) {
            if ((vertex_id_1 * 7 + vertex_id_2 * 13) % 5 == 0) {
                graph_builder.add_edge(vertex_id_1, vertex_id_2);
                expected[vertex_id_1].push_back(vertex_id_2);
                expected[vertex_id_2].push_back(vertex_id_1);
            }
        }
    }
    for (auto& neighbors: expected)
        std::sort(neighbors.begin(), neighbors.end());
    const AdjacencyMatrixGraph graph = graph_builder.build();

    int number_of_threads = 8;
    std::vector<int> number_of_errors(number_of_threads, 0);
    std::vector<std::thread> threads;
    for (int thread_id = 0; thread_id < number_of_threads; ++thread_id) {
        threads.push_back(std::thread([&graph, &expected, &number_of_errors, thread_id, number_of_vertices]()
        {
            for (int repetition = 0; repetition < 20; ++repetition) {
                for (VertexId vertex_id = 0; vertex_id < number_of_vertices; ++vertex_id) {
                    // Each thread visits the vertices in a different order.
                    VertexId v = (vertex_id * (thread_id + 1)) % number_of_vertices;
                    if (sorted_neighbors(graph, v) != expected[v])
                        number_of_errors[thread_id]++;
                }
            }
        }));
    }
    for (std::thread& thread: threads)
        thread.join();
    for (int thread_id = 0; thread_id < number_of_threads; ++thread_id)
        EXPECT_EQ(number_of_errors[thread_id], 0);
}
