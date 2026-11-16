#pragma once

#include "optimizationtools/graph/common.hpp"

#include <vector>
#include <algorithm>

namespace optimizationtools
{

class AdjacencyMatrixGraph
{

public:

    /** Type of the iterators returned by 'neighbors_begin' and 'neighbors_end'. */
    typedef std::vector<VertexId>::const_iterator const_iterator;

    /*
     * Structures
     */

    /**
     * Structure that stores the information for a vertex.
     */
    struct Vertex
    {
        /** Weight of the vertex. */
        Weight weight = 1;

        /** Degree of the vertex. */
        VertexPos degree = 0;
    };

    /*
     * Constructors and destructor
     */

    /*
     * Getters
     */

    inline VertexPos number_of_vertices() const { return vertices_.size(); }

    inline EdgeId number_of_edges() const { return number_of_edges_; }

    inline VertexId degree(VertexId v) const { return vertices_[v].degree; }

    VertexPos highest_degree() const { return highest_degree_; }

    inline Weight weight(VertexId v) const { return vertices_[v].weight; }

    Weight total_weight() const { return total_weight_; };

    /** Get the density of the graph. */
    inline double density() const { return optimizationtools::density(*this); }

    /** Get the average degree of the graph. */
    inline double average_degree() const { return optimizationtools::average_degree(*this); }

    /** Check if vertex index 'vertex_id' is within the correct range. */
    inline void check_vertex_index(VertexId vertex_id) const { optimizationtools::check_vertex_index(*this, vertex_id); }

    /**
     * Get an iterator pointing to the first neighbor of a vertex.
     *
     * The neighbors are computed on the fly and stored in a buffer. Therefore,
     * the iterators of a vertex are invalidated as soon as the neighbors of
     * another vertex are requested, and this method is not thread-safe.
     */
    inline const_iterator neighbors_begin(VertexId vertex_id) const
    {
        compute_neighbors(vertex_id);
        return neighbors_tmp_.begin();
    }

    /** Get an iterator pointing past the last neighbor of a vertex. */
    inline const_iterator neighbors_end(VertexId vertex_id) const
    {
        compute_neighbors(vertex_id);
        return neighbors_tmp_.end();
    }

    /**
     * Return 'true' iff there is an edge between two vertices exists.
     */
    bool has_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2) const
    {
        if (vertex_id_1 == vertex_id_2)
            return false;
        return adjacency_matrix_[(std::max)(vertex_id_1, vertex_id_2)][(std::min)(vertex_id_1, vertex_id_2)];
    }

private:

    /*
     * Private methods
     */

    /** Constructor. */
    inline AdjacencyMatrixGraph() { }

    /** Fill 'neighbors_tmp_' with the neighbors of a vertex. */
    inline void compute_neighbors(VertexId vertex_id) const
    {
        if (vertex_id == vertex_id_tmp_)
            return;
        neighbors_tmp_.clear();
        for (VertexId vertex_id_2 = 0; vertex_id_2 < number_of_vertices(); ++vertex_id_2)
            if (has_edge(vertex_id, vertex_id_2))
                neighbors_tmp_.push_back(vertex_id_2);
        vertex_id_tmp_ = vertex_id;
    }

    /*
     * Private attributes
     */

    /** Vertices. */
    std::vector<Vertex> vertices_;

    /** Adjacency matrix. */
    std::vector<std::vector<bool>> adjacency_matrix_;

    /** Number of edges. */
    EdgeId number_of_edges_ = 0;

    /** Maximum degree. */
    VertexPos highest_degree_ = 0;

    /** Maximum degree. */
    Weight total_weight_ = 0;

    /** Buffer filled by the 'compute_neighbors' method. */
    mutable std::vector<VertexId> neighbors_tmp_;

    /** Vertex whose neighbors are currently stored in 'neighbors_tmp_'. */
    mutable VertexId vertex_id_tmp_ = -1;

    friend class AdjacencyMatrixGraphBuilder;
};

class AdjacencyMatrixGraphBuilder
{

public:

    /** Constructor. */
    AdjacencyMatrixGraphBuilder() { };

    /** Add a vertex. */
    VertexId add_vertex(Weight weight = 1)
    {
        VertexId vertex_id = graph_.vertices_.size();

        AdjacencyMatrixGraph::Vertex vertex;
        vertex.weight = weight;
        graph_.vertices_.push_back(vertex);

        graph_.adjacency_matrix_.push_back(std::vector<bool>(vertex_id));
        return vertex_id;
    }

    /** Add an edge. */
    bool add_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2)
    {
        if (vertex_id_1 == vertex_id_2
                || graph_.has_edge(vertex_id_1, vertex_id_2)) {
            return false;
        }
        graph_.adjacency_matrix_[(std::max)(vertex_id_1, vertex_id_2)][(std::min)(vertex_id_1, vertex_id_2)] = true;
        graph_.number_of_edges_++;
        graph_.vertices_[vertex_id_1].degree++;
        graph_.vertices_[vertex_id_2].degree++;
        return true;
    }

    /** Remove an edge. */
    bool remove_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2)
    {
        if (!graph_.has_edge(vertex_id_1, vertex_id_2))
            return false;
        graph_.adjacency_matrix_[(std::max)(vertex_id_1, vertex_id_2)][(std::min)(vertex_id_1, vertex_id_2)] = false;
        graph_.number_of_edges_--;
        graph_.vertices_[vertex_id_1].degree--;
        graph_.vertices_[vertex_id_2].degree--;
        return true;
    }

    /** Set the weight of a vertex. */
    void set_weight(
            VertexId vertex_id,
            Weight weight)
    {
        graph_.vertices_[vertex_id].weight = weight;
    }


    /*
     * Build
     */

    /** Build. */
    AdjacencyMatrixGraph build()
    {
        for (VertexId vertex_id = 0;
                vertex_id < this->graph_.number_of_vertices();
                ++vertex_id) {
            this->graph_.total_weight_ += this->graph_.weight(vertex_id);
            this->graph_.highest_degree_ = (std::max)(
                    this->graph_.highest_degree_,
                    this->graph_.degree(vertex_id));
        }
        graph_.vertex_id_tmp_ = -1;
        return std::move(graph_);
    }

private:

    /*
     * Private attributes
     */

    /** Graph. */
    AdjacencyMatrixGraph graph_;

};

}
