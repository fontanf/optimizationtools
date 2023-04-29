#pragma once

#include "optimizationtools/graph/abstract_graph.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

#include <cstdint>
#include <vector>

namespace optimizationtools
{

typedef int64_t CliqueId;

class CliqueGraph: public AbstractGraph
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

        /** Cliques to which the vertex belongs. */
        std::vector<CliqueId> cliques;
    };

    /*
     * Constructors and destructor
     */

    /** Create a graph from a file. */
    CliqueGraph(std::string instance_path, std::string format);

    /** Create an empty graph. */
    inline CliqueGraph():
        AbstractGraph(),
        neighbors_tmp_(0) { }

    /** Destructor. */
    inline virtual ~CliqueGraph() { }

    /** Add a vertex. */
    virtual VertexId add_vertex(Weight weight = 1);

    /** Add a new clique. */
    CliqueId add_clique();

    /** Add a new clique. */
    CliqueId add_clique(const std::vector<VertexId>& clique);

    /** Add a vertex to a clique. */
    void add_vertex_to_clique(
            CliqueId clique_id,
            VertexId vertex_id);

    virtual CliqueGraph* clone() const override
    {
        return new CliqueGraph(*this);
    }

    /*
     * Getters
     */

    inline VertexPos number_of_vertices() const override { return vertices_.size(); }

    inline EdgeId number_of_edges() const override { return number_of_edges_; }

    inline VertexId degree(VertexId vertex_id) const override { return vertices_[vertex_id].degree; }

    virtual VertexPos maximum_degree() const override { return maximum_degree_; }

    inline Weight weight(VertexId vertex_id) const override { return vertices_[vertex_id].weight; }

    virtual Weight total_weight() const override { return total_weight_; };

    virtual const_iterator neighbors_begin(VertexId vertex_id) const override
    {
        if (vertex_id != vertex_id_tmp_) {
            neighbors_tmp_.clear();
            for (CliqueId clique_id: vertices_[vertex_id].cliques)
                for (VertexId vertex_id_2: cliques_[clique_id])
                    if (vertex_id_2 != vertex_id)
                        neighbors_tmp_.add(vertex_id_2);
            vertex_id_tmp_ = vertex_id;
        }
        return neighbors_tmp_.begin();
    }

    inline virtual const_iterator neighbors_end(VertexId) const override
    {
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
    VertexPos maximum_degree_ = 0;

    /** Maximum degree. */
    Weight total_weight_ = 0;

    /** Vector filled and returned by the 'adjacency_list' method. */
    mutable IndexedSet neighbors_tmp_;

    /** Last vertex for which method 'neighbors_begin' has been called. */
    mutable VertexId vertex_id_tmp_ = -1;

    /*
     * Private methods
     */

    /** Read a graph in 'default' format. */
    void read_cliquegraph(std::ifstream& file);

};

}

