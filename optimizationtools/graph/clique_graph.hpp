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
        /** Cliques to which the vertex belongs. */
        std::vector<CliqueId> cliques;
    };

    /*
     * Constructors and destructor.
     */

    /** Constructor. */
    inline CliqueGraph():
        AbstractGraph(),
        neighbors_tmp_(0) { }

    /** Destructor. */
    inline virtual ~CliqueGraph() { }

    /** Add a vertex. */
    virtual VertexId add_vertex(Weight weight = 1)
    {
        Vertex vertex;
        vertex.id = vertices_.size();
        vertex.weight = weight;
        vertices_.push_back(vertex);
        total_weight_ += weight;
        return vertex.id;
    }

    /** Add a new clique. */
    CliqueId add_clique()
    {
        CliqueId id = cliques_.size();
        cliques_.push_back({});
        return id;
    }

    /** Add a new clique. */
    CliqueId add_clique(const std::vector<VertexId>& clique)
    {
        CliqueId id = cliques_.size();
        cliques_.push_back(clique);
        for (VertexId v: clique) {
            vertices_[v].degree += clique.size() - 1;
            if (maximum_degree_ < degree(v))
                maximum_degree_ = degree(v);
        }
        number_of_edges_ += clique.size() * (clique.size() - 1) / 2;
        return id;
    }

    /** Add a vertex to a clique. */
    void add_vertex_to_clique(CliqueId clique_id, VertexId v)
    {
        number_of_edges_ += cliques_[clique_id].size();
        for (VertexId v2: cliques_[clique_id])
            vertices_[v2].degree++;
        vertices_[v].degree += cliques_[clique_id].size();
        vertices_[v].cliques.push_back(clique_id);
        cliques_[clique_id].push_back(v);

        for (VertexId v2: cliques_[clique_id])
            if (maximum_degree_ < degree(v2))
                maximum_degree_ = degree(v2);
    }

    virtual CliqueGraph* clone() const override
    {
        return new CliqueGraph(*this);
    }

    /*
     * Getters.
     */

    inline VertexPos number_of_vertices() const override { return vertices_.size(); }

    inline EdgeId number_of_edges() const override { return number_of_edges_; }

    inline VertexId degree(VertexId v) const override { return vertices_[v].degree; }

    virtual VertexPos maximum_degree() const override { return maximum_degree_; }

    inline Weight weight(VertexId v) const override { return vertices_[v].weight; }

    virtual Weight total_weight() const override { return total_weight_; };

    virtual const_iterator neighbors_begin(VertexId v) const override
    {
        neighbors_tmp_.clear();
        for (CliqueId clique_id: vertices_[v].cliques)
            for (VertexId v2: cliques_[clique_id])
                neighbors_tmp_.add(v2);
        return neighbors_tmp_.begin();
    }

    virtual const_iterator neighbors_end(VertexId) const override
    {
        return neighbors_tmp_.end();
    }

private:

    /*
     * Private attributes.
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

};

}

