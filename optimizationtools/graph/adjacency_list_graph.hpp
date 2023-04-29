#pragma once

#include "optimizationtools/graph/abstract_graph.hpp"
#include "optimizationtools/utils/utils.hpp"

namespace optimizationtools
{

class AdjacencyListGraph: public AbstractGraph
{

public:

    /*
     * Structures
     */

    /**
     * Structure that stores the information for an edge incident to a vertex.
     */
    struct VertexEdge
    {
        /** Id of the edge. */
        EdgeId edge_id;

        /** Id of the neighbor. */
        VertexId vertex_id;
    };

    /**
     * Structure that stores the information for a vertex.
     */
    struct Vertex
    {
        /** Weight of the vertex. */
        Weight weight = 1;

        /** Id of the connected component of the vertex. */
        ComponentId component = -1;

        /** Neighbors of the vertex. */
        std::vector<VertexEdge> edges;

        /** Neighbors of the vertex. */
        std::vector<VertexId> neighbors;
    };

    /**
     * Structure that stores the information for an edge.
     */
    struct Edge
    {
        /** First end of the edge. */
        VertexId vertex_id_1;

        /** Second end of the edge. */
        VertexId vertex_id_2;
    };

    /*
     * Constructors destructor
     */

    /** Create a graph from a file. */
    AdjacencyListGraph(
            std::string instance_path,
            std::string format);

    /** Constructor. */
    AdjacencyListGraph(VertexId number_of_vertices = 0);

    /** Add a vertex. */
    virtual VertexId add_vertex(Weight weight = 1);

    /** Set the weight of a vertex. */
    void set_weight(
            VertexId vertex_id,
            Weight weight);

    /** Set the weight of all vertices to 1. */
    void set_unweighted();

    /** Add an edge. */
    EdgeId add_edge(
            VertexId vertex_id_1,
            VertexId vertex_id_2);

    /** Clear graph, i.e. remove vertices and edges. */
    void clear();

    /** Clear the edges of the graph. */
    void clear_edges();

    /** Remove duplicate edges (changes the edge ids). */
    void remove_duplicate_edges();

    /** Create a graph from an AbstractGraph. */
    inline AdjacencyListGraph(
            const AbstractGraph& abstract_graph);

    /** Create the complementary of a graph. */
    AdjacencyListGraph complementary() const;

    virtual AdjacencyListGraph* clone() const override
    {
        return new AdjacencyListGraph(*this);
    }

    /*
     * Getters
     */

    inline VertexPos number_of_vertices() const override { return vertices_.size(); }

    inline virtual VertexPos number_of_edges() const override { return number_of_edges_; }

    inline VertexId degree(VertexId vertex_id) const override { return vertices_[vertex_id].edges.size(); }

    inline virtual VertexPos maximum_degree() const override { return maximum_degree_; }

    inline Weight weight(VertexId vertex_id) const override { return vertices_[vertex_id].weight; }

    virtual Weight total_weight() const override { return total_weight_; };

    const_iterator neighbors_begin(VertexId vertex_id) const override
    {
        return vertices_[vertex_id].neighbors.begin();
    }

    const_iterator neighbors_end(VertexId vertex_id) const override
    {
        return vertices_[vertex_id].neighbors.end();
    }

    /** Get the first end of an edge. */
    inline VertexId first_end(EdgeId edge_id) const { return edges_[edge_id].vertex_id_1; }

    /** Get the second end of an edge.  */
    inline VertexId second_end(EdgeId edge_id) const { return edges_[edge_id].vertex_id_2; }

    /** Get the other end of an edge. */
    inline VertexId other_end(
            EdgeId edge_id,
            VertexId vertex_id) const
    {
        return (vertex_id == edges_[edge_id].vertex_id_1)?
            edges_[edge_id].vertex_id_2:
            edges_[edge_id].vertex_id_1;
    }

    /** Get the list of edges incident to a vertex. */
    inline const std::vector<VertexEdge>& edges(VertexId vertex_id) const { return vertices_[vertex_id].edges; }

    /*
     * Export
     */

    /** Write the graph to a file. */
    void write(std::string instance_path, std::string format);

private:

    /*
     * Private attributes
     */

    /** Vertices. */
    std::vector<Vertex> vertices_;

    /** Edges. */
    std::vector<Edge> edges_;

    /** Number of edges. */
    EdgeId number_of_edges_ = 0;

    /** Maximum degree. */
    VertexPos maximum_degree_ = 0;

    /** Total weight. */
    Weight total_weight_ = 0;

    /*
     * Private methods
     */

    /** Read a graph in 'dimacs1992' format. */
    void read_dimacs1992(std::ifstream& file);

    /** Read a graph in 'dimacs2010' format. */
    void read_dimacs2010(std::ifstream& file);

    /** Read a graph in 'matrixmarket' format. */
    void read_matrixmarket(std::ifstream& file);

    /** Read a graph in 'chaco' format. */
    void read_chaco(std::ifstream& file);

    /** Read a graph in 'snap' format. */
    void read_snap(std::ifstream& file);

    /** Write graph in 'snap' format. */
    void write_snap(std::ofstream& file);

    /** Write graph in 'matrixmarket' format. */
    void write_matrixmarket(std::ofstream& file);

    /** Write graph in 'dimacs' foramt. */
    void write_dimacs(std::ofstream& file);

};

}

