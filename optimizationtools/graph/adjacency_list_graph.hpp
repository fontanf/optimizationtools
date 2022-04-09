#pragma once

#include "optimizationtools/graph/abstract_graph.hpp"
#include "optimizationtools/utils/utils.hpp"

#include <cstdint>
#include <vector>
#include <fstream>

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

    AdjacencyListGraph(std::string instance_path, std::string format)
    {
        std::ifstream file(instance_path);
        if (!file.good())
            throw std::runtime_error(
                    "Unable to open file \"" + instance_path + "\".");

        if (format == "dimacs1992") {
            read_dimacs1992(file);
        } else if (format == "dimacs2010") {
            read_dimacs2010(file);
        } else if (format == "matrixmarket") {
            read_matrixmarket(file);
        } else if (format == "snap") {
            read_snap(file);
        } else if (format == "chaco") {
            read_chaco(file);
        } else {
            throw std::invalid_argument(
                    "Unknown instance format \"" + format + "\".");
        }
    }

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

    /** Set the weight of vertex 'v' to 'weight'. */
    void set_weight(VertexId v, Weight weight)
    {
        total_weight_ -= vertices_[v].weight;
        vertices_[v].weight = weight;
        total_weight_ += vertices_[v].weight;
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

    virtual Weight total_weight() const override { return total_weight_; };

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

    void write_snap(std::ofstream& file)
    {
        for (EdgeId e = 0; e < number_of_edges(); ++e)
            file << first_end(e) << " " << second_end(e) << std::endl;
    }

    void write_matrixmarket(std::ofstream& file)
    {
        file << number_of_vertices()
            << " " << number_of_vertices()
            << " " << number_of_edges()
            << std::endl;
        for (EdgeId e = 0; e < number_of_edges(); ++e)
            file << first_end(e) + 1 << " " << second_end(e) + 1 << std::endl;
    }

    void write_dimacs(std::ofstream& file)
    {
        file << "p edge " << number_of_vertices() << " " << number_of_edges() << std::endl;
        for (EdgeId e = 0; e < number_of_edges(); ++e)
            file << "e " << first_end(e) + 1 << " " << second_end(e) + 1 << std::endl;
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

    /** Total weight. */
    Weight total_weight_ = 0;

    /*
     * Private methods.
     */

    void read_dimacs1992(std::ifstream& file)
    {
        std::string tmp;
        std::vector<std::string> line;

        while (getline(file, tmp)) {
            line = optimizationtools::split(tmp, ' ');
            if (line.size() == 0) {
            } else if (line[0] == "c") {
            } else if (line[0] == "p") {
                VertexId number_of_vertices = stol(line[2]);
                for (VertexId v = 0; v < number_of_vertices; ++v)
                    add_vertex();
            } else if (line[0] == "n") {
                VertexId v = stol(line[1]) - 1;
                Weight w = stol(line[2]);
                set_weight(v, w);
            } else if (line[0] == "e") {
                VertexId v1 = stol(line[1]) - 1;
                VertexId v2 = stol(line[2]) - 1;
                add_edge(v1, v2);
            }
        }
    }

    void read_dimacs2010(std::ifstream& file)
    {
        std::string tmp;
        std::vector<std::string> line;
        bool first = true;
        VertexId v = -1;
        while (v != number_of_vertices()) {
            getline(file, tmp);
            //std::cout << tmp << std::endl;
            line = optimizationtools::split(tmp, ' ');
            if (tmp[0] == '%')
                continue;
            if (first) {
                VertexId number_of_vertices = stol(line[0]);
                for (VertexId v = 0; v < number_of_vertices; ++v)
                    add_vertex();
                first = false;
                v = 0;
            } else {
                for (std::string str: line) {
                    VertexId v2 = stol(str) - 1;
                    if (v2 > v)
                        add_edge(v, v2);
                }
                v++;
            }
        }
    }

    void read_matrixmarket(std::ifstream& file)
    {
        std::string tmp;
        std::vector<std::string> line;
        do {
            getline(file, tmp);
        } while (tmp[0] == '%');
        std::stringstream ss(tmp);
        VertexId n = -1;
        ss >> n;
        for (VertexId v = 0; v < n; ++v)
            add_vertex();

        VertexId v1 = -1;
        VertexId v2 = -1;
        while (getline(file, tmp)) {
            std::stringstream ss(tmp);
            ss >> v1 >> v2;
            add_edge(v1 - 1, v2 - 1);
        }
    }

    void read_chaco(std::ifstream& file)
    {
        std::string tmp;
        std::vector<std::string> line;

        getline(file, tmp);
        line = optimizationtools::split(tmp, ' ');
        VertexId number_of_vertices = stol(line[0]);
        vertices_.resize(number_of_vertices);

        for (VertexId v = 0; v < number_of_vertices; ++v) {
            getline(file, tmp);
            line = optimizationtools::split(tmp, ' ');
            for (std::string str: line) {
                VertexId v2 = stol(str) - 1;
                if (v2 > v)
                    add_edge(v, v2);
            }
        }
    }

    void read_snap(std::ifstream& file)
    {
        std::string tmp;
        std::vector<std::string> line;
        do {
            getline(file, tmp);
        } while (tmp[0] == '#');

        VertexId v1 = -1;
        VertexId v2 = -1;
        for (;;) {
            file >> v1 >> v2;
            if (file.eof())
                break;
            while (std::max(v1, v2) >= number_of_vertices())
                add_vertex();
            add_edge(v1, v2);
        }
    }

};

}

