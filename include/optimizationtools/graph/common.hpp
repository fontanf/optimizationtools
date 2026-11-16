#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include <functional>
#include <utility>

namespace optimizationtools
{

/**
 * Graph classes don't share a common base class. Algorithms that should work
 * with several graph types take the graph type as a template parameter.
 *
 * A graph type 'Graph' is expected to provide:
 * - VertexPos number_of_vertices() const
 * - EdgeId number_of_edges() const
 * - VertexPos degree(VertexId) const
 * - VertexPos highest_degree() const
 * - Weight weight(VertexId) const
 * - Weight total_weight() const
 * - Graph::const_iterator neighbors_begin(VertexId) const
 * - Graph::const_iterator neighbors_end(VertexId) const
 *
 * To store a graph of any type in a class which is not a template, such as a
 * problem instance, use the 'Graph' class from 'graph/graph.hpp'.
 */

typedef int64_t VertexId;
typedef int64_t VertexPos;
typedef int64_t EdgeId;
typedef int64_t ComponentId;
typedef int64_t Counter;
typedef double Weight;

struct PairHash
{
    std::size_t operator()(const std::pair<VertexId, VertexId>& p) const
    {
        return std::hash<VertexId>()(p.first) ^ (std::hash<VertexId>()(p.second) << 1);
    }
};

/** Get the density of a graph. */
template <typename Graph>
inline double density(const Graph& graph)
{
    return (double)graph.number_of_edges() * 2
        / graph.number_of_vertices()
        / graph.number_of_vertices();
}

/** Get the average degree of a graph. */
template <typename Graph>
inline double average_degree(const Graph& graph)
{
    return (double)graph.number_of_edges() * 2 / graph.number_of_vertices();
}

/** Check if a vertex index is within the correct range. */
template <typename Graph>
inline void check_vertex_index(
        const Graph& graph,
        VertexId vertex_id)
{
    if (vertex_id < 0 || vertex_id >= graph.number_of_vertices())
        throw std::out_of_range(
                "Invalid vertex index: \"" + std::to_string(vertex_id) + "\"."
                + " Vertex indices should belong to [0, "
                + std::to_string(graph.number_of_vertices() - 1) + "].");
}

}
