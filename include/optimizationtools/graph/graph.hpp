#pragma once

#include "optimizationtools/graph/adjacency_list_graph.hpp"
#include "optimizationtools/graph/adjacency_matrix_graph.hpp"
#include "optimizationtools/graph/clique_graph.hpp"

#include <memory>

namespace optimizationtools
{

/**
 * Call 'function' with the concrete graph stored in 'graph' as first
 * argument.
 *
 * 'function' is typically a function template. It must return the same type
 * for all graph types.
 */
#define FUNCTION_WITH_GRAPH_0(function, graph) \
    (((graph).adjacency_list_graph() != nullptr)? \
    (function)(*(graph).adjacency_list_graph()): \
    ((graph).adjacency_matrix_graph() != nullptr)? \
    (function)(*(graph).adjacency_matrix_graph()): \
    (function)(*(graph).clique_graph()))

/**
 * Call 'function' with the concrete graph stored in 'graph' as first
 * argument, followed by the other arguments.
 */
#define FUNCTION_WITH_GRAPH(function, graph, ...) \
    (((graph).adjacency_list_graph() != nullptr)? \
    (function)(*(graph).adjacency_list_graph(), __VA_ARGS__): \
    ((graph).adjacency_matrix_graph() != nullptr)? \
    (function)(*(graph).adjacency_matrix_graph(), __VA_ARGS__): \
    (function)(*(graph).clique_graph(), __VA_ARGS__))

/**
 * Class that stores a graph of any of the graph types.
 *
 * It allows classes such as problem instances to store a graph without being
 * templates. Algorithms get the concrete graph with the 'FUNCTION_WITH_GRAPH'
 * macros and are implemented as templates on the graph type, so that the
 * graph type is known when iterating over the neighbors of a vertex.
 *
 * Copying a Graph doesn't copy the underlying graph.
 */
class Graph
{

public:

    /*
     * Constructors and destructor
     */

    /** Create a graph from an AdjacencyListGraph. */
    Graph(const std::shared_ptr<const AdjacencyListGraph>& adjacency_list_graph):
        adjacency_list_graph_(adjacency_list_graph)
    {
        check_not_null(adjacency_list_graph_.get());
    }

    /** Create a graph from an AdjacencyMatrixGraph. */
    Graph(const std::shared_ptr<const AdjacencyMatrixGraph>& adjacency_matrix_graph):
        adjacency_matrix_graph_(adjacency_matrix_graph)
    {
        check_not_null(adjacency_matrix_graph_.get());
    }

    /** Create a graph from a CliqueGraph. */
    Graph(const std::shared_ptr<const CliqueGraph>& clique_graph):
        clique_graph_(clique_graph)
    {
        check_not_null(clique_graph_.get());
    }

    /*
     * Getters
     */

    /** Get the AdjacencyListGraph, 'nullptr' if the graph has another type. */
    inline const AdjacencyListGraph* adjacency_list_graph() const { return adjacency_list_graph_.get(); }

    /** Get the AdjacencyMatrixGraph, 'nullptr' if the graph has another type. */
    inline const AdjacencyMatrixGraph* adjacency_matrix_graph() const { return adjacency_matrix_graph_.get(); }

    /** Get the CliqueGraph, 'nullptr' if the graph has another type. */
    inline const CliqueGraph* clique_graph() const { return clique_graph_.get(); }

    /** Get the name of the type of the graph. */
    inline std::string type() const
    {
        if (adjacency_list_graph_ != nullptr)
            return "AdjacencyListGraph";
        if (adjacency_matrix_graph_ != nullptr)
            return "AdjacencyMatrixGraph";
        return "CliqueGraph";
    }

    /** Get the number of vertices. */
    inline VertexPos number_of_vertices() const { return FUNCTION_WITH_GRAPH_0(get_number_of_vertices, *this); }

    /** Get the number of edges. */
    inline EdgeId number_of_edges() const { return FUNCTION_WITH_GRAPH_0(get_number_of_edges, *this); }

    /** Get the degree of a vertex. */
    inline VertexPos degree(VertexId vertex_id) const { return FUNCTION_WITH_GRAPH(get_degree, *this, vertex_id); }

    /** Get the highest degree of the graph. */
    inline VertexPos highest_degree() const { return FUNCTION_WITH_GRAPH_0(get_highest_degree, *this); }

    /** Get the weight of a vertex. */
    inline Weight weight(VertexId vertex_id) const { return FUNCTION_WITH_GRAPH(get_weight, *this, vertex_id); }

    /** Get the total weight of the graph. */
    inline Weight total_weight() const { return FUNCTION_WITH_GRAPH_0(get_total_weight, *this); }

    /** Get the density of the graph. */
    inline double density() const { return optimizationtools::density(*this); }

    /** Get the average degree of the graph. */
    inline double average_degree() const { return optimizationtools::average_degree(*this); }

    /** Check if vertex index 'vertex_id' is within the correct range. */
    inline void check_vertex_index(VertexId vertex_id) const { optimizationtools::check_vertex_index(*this, vertex_id); }

    /*
     * Conversions
     */

    /**
     * Get the graph as an AdjacencyListGraph.
     *
     * If the graph is already an AdjacencyListGraph, it is returned without
     * copy. Otherwise, a new AdjacencyListGraph is built.
     */
    inline std::shared_ptr<const AdjacencyListGraph> to_adjacency_list_graph() const
    {
        if (adjacency_list_graph_ != nullptr)
            return adjacency_list_graph_;
        if (adjacency_matrix_graph_ != nullptr)
            return std::shared_ptr<const AdjacencyListGraph>(new AdjacencyListGraph(*adjacency_matrix_graph_));
        return std::shared_ptr<const AdjacencyListGraph>(new AdjacencyListGraph(*clique_graph_));
    }

    /**
     * Get the graph as an AdjacencyMatrixGraph.
     *
     * If the graph is already an AdjacencyMatrixGraph, it is returned without
     * copy. Otherwise, a new AdjacencyMatrixGraph is built.
     */
    inline std::shared_ptr<const AdjacencyMatrixGraph> to_adjacency_matrix_graph() const
    {
        if (adjacency_matrix_graph_ != nullptr)
            return adjacency_matrix_graph_;
        if (adjacency_list_graph_ != nullptr)
            return std::shared_ptr<const AdjacencyMatrixGraph>(new AdjacencyMatrixGraph(*adjacency_list_graph_));
        return std::shared_ptr<const AdjacencyMatrixGraph>(new AdjacencyMatrixGraph(*clique_graph_));
    }

private:

    /*
     * Private methods
     */

    inline static void check_not_null(const void* graph)
    {
        if (graph == nullptr)
            throw std::invalid_argument("optimizationtools::Graph: the graph is null.");
    }

    template <typename G> static VertexPos get_number_of_vertices(const G& graph) { return graph.number_of_vertices(); }
    template <typename G> static EdgeId get_number_of_edges(const G& graph) { return graph.number_of_edges(); }
    template <typename G> static VertexPos get_degree(const G& graph, VertexId vertex_id) { return graph.degree(vertex_id); }
    template <typename G> static VertexPos get_highest_degree(const G& graph) { return graph.highest_degree(); }
    template <typename G> static Weight get_weight(const G& graph, VertexId vertex_id) { return graph.weight(vertex_id); }
    template <typename G> static Weight get_total_weight(const G& graph) { return graph.total_weight(); }

    /*
     * Private attributes
     */

    /** Adjacency list graph. */
    std::shared_ptr<const AdjacencyListGraph> adjacency_list_graph_ = nullptr;

    /** Adjacency matrix graph. */
    std::shared_ptr<const AdjacencyMatrixGraph> adjacency_matrix_graph_ = nullptr;

    /** Clique graph. */
    std::shared_ptr<const CliqueGraph> clique_graph_ = nullptr;

};

}
