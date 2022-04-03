#pragma once

#include "optimizationtools/graph/abstract_graph.hpp"

#include <cstdint>
#include <vector>

namespace optimizationtools
{

class AdjacencyListGraph: public AbstractGraph
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
        /** Id of the connected component of the vertex. */
        ComponentId component = -1;
        /** Neighbors of the vertex. */
        std::vector<EdgeId> edges;
        /** Neighbors of the vertex. */
        std::vector<VertexId> neighbors;
    };

    /**
     * Structure that stores the information for an edge.
     */
    struct Edge
    {
        /** Unique id of the edge. */
        EdgeId id;
        /** First end of the edge. */
        VertexId v1;
        /** Second end of the edge. */
        VertexId v2;
    };

    /*
     * Constructors destructor.
     */

    /** Constructor. */
    inline AdjacencyListGraph() { }

    /** Add a vertex. */
    virtual VertexId add_vertex(Weight weight = 1)
    {
        Vertex vertex;
        vertex.id = vertices_.size();
        vertex.weight = weight;
        vertices_.push_back(vertex);
        return vertex.id;
    }

    /** Set the weight of vertex 'v' to 'weight'. */
    void set_weight(VertexId v, Weight weight)
    {
        vertices_[v].weight = weight;
    };

    /** Set the weight of all vertices to 1. */
    void set_unweighted()
    {
        for (VertexId v = 0; v < number_of_vertices(); ++v)
            set_weight(v, 1);
    }

    /** Add an edge. */
    EdgeId add_edge(VertexId v1, VertexId v2)
    {
        if (v1 == v2) {
            return -1;
        }

        Edge edge;
        edge.id = edges_.size();
        edge.v1 = v1;
        edge.v2 = v2;
        edges_.push_back(edge);

        vertices_[v1].edges.push_back(edge.id);
        vertices_[v1].neighbors.push_back(v2);

        vertices_[v2].edges.push_back(edge.id);
        vertices_[v2].neighbors.push_back(v1);

        number_of_edges_++;
        if (maximum_degree_ < std::max(degree(v1), degree(v2)))
            maximum_degree_ = std::max(degree(v1), degree(v2));

        return edge.id;
    }

    /** Create a graph from an AbstractGraph. */
    inline AdjacencyListGraph(
            const AbstractGraph& abstract_graph)
    {
        for (VertexId v = 0; v < abstract_graph.number_of_vertices(); ++v)
            add_vertex();
        for (VertexId v = 0; v < abstract_graph.number_of_vertices(); ++v) {
            for (auto it = abstract_graph.neighbors_begin(v);
                    it != abstract_graph.neighbors_end(v); ++it) {
                if (v > *it)
                    add_edge(v, *it);
            }
        }
    }

    virtual AdjacencyListGraph* clone() const override
    {
        return new AdjacencyListGraph(*this);
    }

    /*
     * Getters.
     */

    inline VertexPos number_of_vertices() const override { return vertices_.size(); }

    virtual VertexPos number_of_edges() const override { return number_of_edges_; }

    inline VertexId degree(VertexId v) const override { return vertices_[v].edges.size(); }

    virtual VertexPos maximum_degree() const override { return maximum_degree_; }

    inline Weight weight(VertexId v) const override { return vertices_[v].weight; }

    const_iterator neighbors_begin(VertexId v) const override
    {
        return vertices_[v].neighbors.begin();
    }

    const_iterator neighbors_end(VertexId v) const override
    {
        return vertices_[v].neighbors.end();
    }

    /** Get the first end of edge 'e'. */
    inline VertexId first_end(EdgeId e) const { return edges_[e].v1; }

    /** Get the second end of edge 'e'.  */
    inline VertexId second_end(EdgeId e) const { return edges_[e].v2; }

    /** Get the end of edge 'e' which is not 'v'. */
    inline VertexId other_end(EdgeId e, VertexId v) const
    {
        return (v == edges_[e].v1)? edges_[e].v2: edges_[e].v1;
    }

private:

    /*
     * Private attributes.
     */

    /** Vertices. */
    std::vector<Vertex> vertices_;

    /** Edges. */
    std::vector<Edge> edges_;

    /** Number of edges. */
    EdgeId number_of_edges_ = 0;

    /** Maximum degree. */
    VertexPos maximum_degree_ = 0;

};

}

