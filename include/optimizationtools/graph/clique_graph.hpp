#pragma once

#include "optimizationtools/graph/common.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

#include <vector>
#include <string>
#include <algorithm>

namespace optimizationtools
{

typedef int64_t CliqueId;

class CliqueGraph
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

        /** Cliques to which the vertex belongs. */
        std::vector<CliqueId> cliques;
    };

    /*
     * Constructors and destructor
     */

    /*
     * Getters
     */

    inline VertexPos number_of_vertices() const { return vertices_.size(); }

    inline EdgeId number_of_edges() const { return number_of_edges_; }

    inline VertexId degree(VertexId vertex_id) const { return vertices_[vertex_id].degree; }

    VertexPos highest_degree() const { return highest_degree_; }

    inline Weight weight(VertexId vertex_id) const { return vertices_[vertex_id].weight; }

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

private:

    /*
     * Private attributes
     */

    /** Vertices. */
    std::vector<Vertex> vertices_;

    /** Cliques. */
    std::vector<std::vector<VertexId>> cliques_;

    /** Number of edges. */
    EdgeId number_of_edges_ = 0;

    /** Maximum degree. */
    VertexPos highest_degree_ = 0;

    /** Maximum degree. */
    Weight total_weight_ = 0;

    /** Vector filled and returned by the 'adjacency_list' method. */
    mutable IndexedSet neighbors_tmp_;

    /** Vertex whose neighbors are currently stored in 'neighbors_tmp_'. */
    mutable VertexId vertex_id_tmp_ = -1;

    /*
     * Private methods
     */

    /** Create an empty graph. */
    inline CliqueGraph():
        neighbors_tmp_(0) { }

    /** Fill 'neighbors_tmp_' with the neighbors of a vertex. */
    inline void compute_neighbors(VertexId vertex_id) const
    {
        if (vertex_id == vertex_id_tmp_)
            return;
        neighbors_tmp_.clear();
        for (CliqueId clique_id: vertices_[vertex_id].cliques)
            for (VertexId vertex_id_2: cliques_[clique_id])
                if (vertex_id_2 != vertex_id)
                    neighbors_tmp_.add(vertex_id_2);
        vertex_id_tmp_ = vertex_id;
    }

    friend class CliqueGraphBuilder;
};

class CliqueGraphBuilder
{

public:

    /** Constructor. */
    CliqueGraphBuilder() { };

    /** Create a graph from a file. */
    void read(
            const std::string& instance_path,
            const std::string& format);

    /** Add a vertex. */
    VertexId add_vertex(Weight weight = 1);

    /** Add a new clique. */
    CliqueId add_clique();

    /** Add a new clique. */
    CliqueId add_clique(const std::vector<VertexId>& clique);

    /** Add a vertex to a clique. */
    void add_vertex_to_clique(
            CliqueId clique_id,
            VertexId vertex_id);

    /** Set the weight of a vertex. */
    void set_weight(
            VertexId vertex_id,
            Weight weight);

    /*
     * Build
     */

    /** Build. */
    CliqueGraph build()
    {
        for (VertexId vertex_id = 0;
                vertex_id < this->graph_.number_of_vertices();
                ++vertex_id) {
            this->graph_.total_weight_ += this->graph_.weight(vertex_id);
            this->graph_.highest_degree_ = (std::max)(
                    this->graph_.highest_degree_,
                    this->graph_.degree(vertex_id));
        }
        graph_.neighbors_tmp_ = optimizationtools::IndexedSet(graph_.number_of_vertices());
        graph_.vertex_id_tmp_ = -1;
        return std::move(graph_);
    }

private:

    /*
     * Private attributes
     */

    /** Graph. */
    CliqueGraph graph_;

    /*
     * Private methods
     */

    /** Read a graph in 'default' format. */
    void read_cliquegraph(std::ifstream& file);

};

}
