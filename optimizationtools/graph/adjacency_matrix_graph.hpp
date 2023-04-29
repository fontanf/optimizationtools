#pragma once

#include "optimizationtools/graph/abstract_graph.hpp"

#include <cstdint>
#include <vector>

namespace optimizationtools
{

class AdjacencyMatrixGraph: public AbstractGraph
{

public:

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

    /** Constructor. */
    inline AdjacencyMatrixGraph(): AbstractGraph() { }

    /** Destructor. */
    inline virtual ~AdjacencyMatrixGraph() { }

    /** Add a vertex. */
    VertexId add_vertex(Weight weight = 1)
    {
        VertexId vertex_id = vertices_.size();

        Vertex vertex;
        vertex.weight = weight;
        vertices_.push_back(vertex);

        adjacency_matrix_.push_back(std::vector<bool>(vertex_id));
        total_weight_ += weight;
        return vertex_id;
    }

    /** Add an edge. */
    void add_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2)
    {
        adjacency_matrix_[std::max(vertex_id_1, vertex_id_2)][std::min(vertex_id_1, vertex_id_2)] = true;
        number_of_edges_++;
        vertices_[vertex_id_1].degree++;
        vertices_[vertex_id_2].degree++;
    }

    /** Remove an edge. */
    void remove_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2)
    {
        adjacency_matrix_[std::max(vertex_id_1, vertex_id_2)][std::min(vertex_id_1, vertex_id_2)] = false;
        number_of_edges_--;
        vertices_[vertex_id_1].degree--;
        vertices_[vertex_id_2].degree--;
    }

    virtual AdjacencyMatrixGraph* clone() const override
    {
        return new AdjacencyMatrixGraph(*this);
    }

    /*
     * Getters
     */

    inline VertexPos number_of_vertices() const override { return vertices_.size(); }

    inline EdgeId number_of_edges() const override { return number_of_edges_; }

    inline VertexId degree(VertexId v) const override { return vertices_[v].degree; }

    virtual VertexPos maximum_degree() const override { return maximum_degree_; }

    inline Weight weight(VertexId v) const override { return vertices_[v].weight; }

    virtual Weight total_weight() const override { return total_weight_; };

    virtual const_iterator neighbors_begin(VertexId vertex_id) const override
    {
        neighbors_tmp_.clear();
        for (VertexId vertex_id_2 = 0; vertex_id_2 < number_of_vertices(); ++vertex_id_2)
            if (has_edge(vertex_id, vertex_id_2))
                neighbors_tmp_.push_back(vertex_id_2);
        return neighbors_tmp_.begin();
    }

    virtual const_iterator neighbors_end(VertexId) const override
    {
        return neighbors_tmp_.end();
    }

    /**
     * Return 'true' iff there is an edge between two vertices exists.
     */
    bool has_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2) const
    {
        return adjacency_matrix_[std::max(vertex_id_1, vertex_id_2)][std::min(vertex_id_1, vertex_id_2)];
    }

private:

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
    VertexPos maximum_degree_ = 0;

    /** Maximum degree. */
    Weight total_weight_ = 0;

    /** Vector filled and returned by the 'adjacency_list' method. */
    mutable std::vector<VertexId> neighbors_tmp_;

};

}

