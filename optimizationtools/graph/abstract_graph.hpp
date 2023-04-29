#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

namespace optimizationtools
{

typedef int64_t VertexId;
typedef int64_t VertexPos;
typedef int64_t EdgeId;
typedef int64_t ComponentId;
typedef int64_t Counter;
typedef double Weight;

class AbstractGraph
{

public:

    /*
     * Constructors and destructor
     */

    /** Constructor. */
    inline AbstractGraph() { }

    /** Destructor. */
    inline virtual ~AbstractGraph() { }

    /** Clone. */
    virtual AbstractGraph* clone() const = 0;

    /*
     * Getters
     */

    /** Get the number of vertices. */
    virtual VertexPos number_of_vertices() const = 0;

    /** Get the number of edges. */
    virtual VertexPos number_of_edges() const = 0;

    /** Get the degree of vertex 'v'. */
    virtual VertexPos degree(VertexId v) const = 0;

    /** Get the maximum degree of the graph. */
    virtual VertexPos maximum_degree() const = 0;

    /** Get the weight of vertex 'v'. */
    virtual Weight weight(VertexId v) const = 0;

    /** Get the total weight of the graph. */
    virtual Weight total_weight() const = 0;

    typedef typename std::vector<VertexId>::const_iterator const_iterator;

    /**
     * Returns an iterator pointing to the first element in the neighbors of
     * vertex 'v'.
     */
    virtual const_iterator neighbors_begin(VertexId vertex_id) const = 0;

    /**
     * Returns an iterator referring to the past-the-end element in neighbors
     * of vertex 'v'.
     */
    virtual const_iterator neighbors_end(VertexId vertex_id) const = 0;

    /*
     * Checkers
     */

    /** Check if vertex index 'v' is within the correct range. */
    inline void check_vertex_index(VertexId vertex_id) const
    {
        if (vertex_id < 0 || vertex_id >= number_of_vertices())
            throw std::out_of_range(
                    "Invalid vertex index: \"" + std::to_string(vertex_id) + "\"."
                    + " Vertex indices should belong to [0, "
                    + std::to_string(number_of_vertices() - 1) + "].");
    }

};

}

