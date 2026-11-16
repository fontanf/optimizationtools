#pragma once

#include "optimizationtools/graph/common.hpp"

#include <vector>
#include <algorithm>
#include <iterator>
#include <cstddef>

namespace optimizationtools
{

class AdjacencyMatrixGraph
{

public:

    /**
     * Iterator over the neighbors of a vertex.
     *
     * It walks the row of the vertex in the adjacency matrix and skips the
     * non-neighbors. It doesn't store anything in the graph, so iterating
     * over the neighbors of several vertices at the same time, from one or
     * several threads, is safe.
     */
    class const_iterator
    {

    public:

        typedef std::forward_iterator_tag iterator_category;
        typedef VertexId value_type;
        typedef std::ptrdiff_t difference_type;
        typedef const VertexId* pointer;
        typedef const VertexId& reference;

        /** Constructor. */
        const_iterator(
                const AdjacencyMatrixGraph* graph,
                VertexId vertex_id,
                VertexId neighbor_id):
            graph_(graph),
            vertex_id_(vertex_id),
            neighbor_id_(neighbor_id)
        {
            skip_non_neighbors();
        }

        inline reference operator*() const { return neighbor_id_; }

        inline pointer operator->() const { return &neighbor_id_; }

        inline const_iterator& operator++()
        {
            ++neighbor_id_;
            skip_non_neighbors();
            return *this;
        }

        inline const_iterator operator++(int)
        {
            const_iterator it = *this;
            ++(*this);
            return it;
        }

        inline bool operator==(const const_iterator& it) const { return neighbor_id_ == it.neighbor_id_; }

        inline bool operator!=(const const_iterator& it) const { return neighbor_id_ != it.neighbor_id_; }

    private:

        /** Move to the next neighbor, or to the end. */
        inline void skip_non_neighbors()
        {
            while (neighbor_id_ < graph_->number_of_vertices()
                    && !graph_->has_edge(vertex_id_, neighbor_id_)) {
                ++neighbor_id_;
            }
        }

        /** Graph. */
        const AdjacencyMatrixGraph* graph_;

        /** Vertex whose neighbors are iterated over. */
        VertexId vertex_id_;

        /** Current neighbor; 'number_of_vertices()' for the end iterator. */
        VertexId neighbor_id_;

    };

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
     * Iterating over all the neighbors of a vertex takes
     * O(number_of_vertices()).
     */
    inline const_iterator neighbors_begin(VertexId vertex_id) const
    {
        return const_iterator(this, vertex_id, 0);
    }

    /** Get an iterator pointing past the last neighbor of a vertex. */
    inline const_iterator neighbors_end(VertexId vertex_id) const
    {
        return const_iterator(this, vertex_id, number_of_vertices());
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
