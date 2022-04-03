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
     * Structures.
     */

    /**
     * Structure that stores the information for a vertex.
     */
    struct Vertex
    {
        /** Unique id of the vertex. */
        VertexId id;
        /** Weight of the vertex. */
        Weight weight = 1;
        /** Degree of the vertex. */
        VertexPos degree = 0;
    };

    /*
     * Constructors and destructor.
     */

    /** Constructor. */
    inline AdjacencyMatrixGraph(): AbstractGraph() { }

    /** Destructor. */
    inline virtual ~AdjacencyMatrixGraph() { }

    /** Add a vertex. */
    VertexId add_vertex(Weight weight = 1)
    {
        Vertex vertex;
        vertex.id = vertices_.size();
        vertex.weight = weight;
        vertices_.push_back(vertex);
        adjacency_matrix_.push_back(std::vector<bool>(vertex.id));
        return vertex.id;
    }

    /** Add an edge. */
    void add_edge(VertexId v1, VertexId v2)
    {
        adjacency_matrix_[std::max(v1, v2)][std::min(v1, v2)] = true;
        number_of_edges_++;
        vertices_[v1].degree++;
        vertices_[v2].degree++;
    }

    /** Remove an edge. */
    void remove_edge(VertexId v1, VertexId v2)
    {
        adjacency_matrix_[std::max(v1, v2)][std::min(v1, v2)] = false;
        number_of_edges_--;
        vertices_[v1].degree--;
        vertices_[v2].degree--;
    }

    virtual AdjacencyMatrixGraph* clone() const override
    {
        return new AdjacencyMatrixGraph(*this);
    }

    /*
     * Getters.
     */

    inline VertexPos number_of_vertices() const override { return vertices_.size(); }

    inline EdgeId number_of_edges() const override { return number_of_edges_; }

    inline VertexId degree(VertexId v) const override { return vertices_[v].degree; }

    virtual VertexPos maximum_degree() const override { return maximum_degree_; }

    inline Weight weight(VertexId v) const override { return vertices_[v].weight; }

    virtual const_iterator neighbors_begin(VertexId v) const override
    {
        neighbors_tmp_.clear();
        for (VertexId v2 = 0; v2 < number_of_vertices(); ++v2)
            if (has_edge(v, v2))
                neighbors_tmp_.push_back(v2);
        return neighbors_tmp_.begin();
    }

    virtual const_iterator neighbors_end(VertexId) const override
    {
        return neighbors_tmp_.end();
    }

    /**
     * Return 'true' iff there is an edge between vertex 'v1' and vertex 'v2'.
     */
    bool has_edge(VertexId v1, VertexId v2) const
    {
        return adjacency_matrix_[std::max(v1, v2)][std::min(v1, v2)];
    }

private:

    /*
     * Private attributes.
     */

    /** Vertices. */
    std::vector<Vertex> vertices_;

    /** Adjacency matrix. */
    std::vector<std::vector<bool>> adjacency_matrix_;

    /** Number of edges. */
    EdgeId number_of_edges_ = 0;

    /** Maximum degree. */
    VertexPos maximum_degree_ = 0;

    /** Vector filled and returned by the 'adjacency_list' method. */
    mutable std::vector<VertexId> neighbors_tmp_;

};

}

