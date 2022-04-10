#include "optimizationtools/graph/adjacency_list_graph.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

#include <cstdint>
#include <vector>
#include <fstream>

using namespace optimizationtools;

AdjacencyListGraph::AdjacencyListGraph(
        std::string instance_path,
        std::string format)
{
    std::ifstream file(instance_path);
    if (!file.good())
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");

    if (format == "dimacs" || format == "dimacs1992") {
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

VertexId AdjacencyListGraph::add_vertex(Weight weight)
{
    Vertex vertex;
    vertex.id = vertices_.size();
    vertex.weight = weight;
    vertices_.push_back(vertex);
    total_weight_ += weight;
    return vertex.id;
}

void AdjacencyListGraph::set_weight(VertexId v, Weight weight)
{
    total_weight_ -= vertices_[v].weight;
    vertices_[v].weight = weight;
    total_weight_ += vertices_[v].weight;
};

void AdjacencyListGraph::set_unweighted()
{
    for (VertexId v = 0; v < number_of_vertices(); ++v)
        set_weight(v, 1);
}

EdgeId AdjacencyListGraph::add_edge(VertexId v1, VertexId v2)
{
    if (v1 == v2) {
        return -1;
    }

    Edge edge;
    edge.id = edges_.size();
    edge.v1 = v1;
    edge.v2 = v2;
    edges_.push_back(edge);

    VertexEdge ve1;
    ve1.e = edge.id;
    ve1.v = v2;
    vertices_[v1].edges.push_back(ve1);
    vertices_[v1].neighbors.push_back(v2);

    VertexEdge ve2;
    ve2.e = edge.id;
    ve2.v = v1;
    vertices_[v2].edges.push_back(ve2);
    vertices_[v2].neighbors.push_back(v1);

    number_of_edges_++;
    if (maximum_degree_ < std::max(degree(v1), degree(v2)))
        maximum_degree_ = std::max(degree(v1), degree(v2));

    return edge.id;
}

void AdjacencyListGraph::clear()
{
    vertices_.clear();
    edges_.clear();
    number_of_edges_ = 0;
    maximum_degree_ = 0;
    total_weight_ = 0;
}

void AdjacencyListGraph::clear_edges()
{
    edges_.clear();
    maximum_degree_ = 0;
    number_of_edges_ = 0;
    for (VertexId v = 0; v < number_of_vertices(); ++v) {
        vertices_[v].edges.clear();
        vertices_[v].neighbors.clear();
    }
}

void AdjacencyListGraph::remove_duplicate_edges()
{
    std::vector<std::vector<VertexId>> neighbors(number_of_vertices());
    for (VertexId v = 0; v < number_of_vertices(); ++v) {
        for (VertexId v_neighbor: vertices_[v].neighbors)
            if (v_neighbor > v)
                neighbors[v].push_back(v_neighbor);
        sort(neighbors[v].begin(), neighbors[v].end());
        neighbors[v].erase(
                std::unique(
                    neighbors[v].begin(),
                    neighbors[v].end()),
                neighbors[v].end());
    }
    clear_edges();
    for (VertexId v1 = 0; v1 < number_of_vertices(); ++v1)
        for (VertexId v2: neighbors[v1])
            add_edge(v1, v2);
}

AdjacencyListGraph::AdjacencyListGraph(VertexId number_of_vertices)
{
    for (VertexId v = 0; v < number_of_vertices; ++v)
        add_vertex();
}

AdjacencyListGraph AdjacencyListGraph::complementary() const
{
    AdjacencyListGraph graph(number_of_vertices());
    optimizationtools::IndexedSet neighbors(number_of_vertices());
    for (VertexId v = 0; v < number_of_vertices(); ++v) {
        neighbors.clear();
        for (auto it = neighbors_begin(v);
                it != neighbors_end(v); ++it) {
            neighbors.add(*it);
        }
        for (auto it = neighbors.out_begin(); it != neighbors.out_end(); ++it)
            if (*it > v)
                graph.add_edge(v, *it);
    }
    return graph;
}

AdjacencyListGraph::AdjacencyListGraph(
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

void AdjacencyListGraph::read_dimacs1992(std::ifstream& file)
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

void AdjacencyListGraph::read_dimacs2010(std::ifstream& file)
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

void AdjacencyListGraph::read_matrixmarket(std::ifstream& file)
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

void AdjacencyListGraph::read_chaco(std::ifstream& file)
{
    std::string tmp;
    std::vector<std::string> line;

    getline(file, tmp);
    line = optimizationtools::split(tmp, ' ');
    VertexId number_of_vertices = stol(line[0]);
    for (VertexId v = 0; v < number_of_vertices; ++v)
        add_vertex();

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

void AdjacencyListGraph::read_snap(std::ifstream& file)
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

void AdjacencyListGraph::write(
        std::string instance_path,
        std::string format)
{
    std::ofstream file(instance_path);
    if (!file.good())
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");

    if (format == "dimacs") {
        write_dimacs(file);
    } else if (format == "matrixmarket") {
        write_matrixmarket(file);
    } else if (format == "snap") {
        write_snap(file);
    } else {
        throw std::invalid_argument(
                "Unknown instance format \"" + format + "\".");
    }
}

void AdjacencyListGraph::write_snap(std::ofstream& file)
{
    for (EdgeId e = 0; e < number_of_edges(); ++e)
        file << first_end(e) << " " << second_end(e) << std::endl;
}

void AdjacencyListGraph::write_matrixmarket(std::ofstream& file)
{
    file << number_of_vertices()
        << " " << number_of_vertices()
        << " " << number_of_edges()
        << std::endl;
    for (EdgeId e = 0; e < number_of_edges(); ++e)
        file << first_end(e) + 1 << " " << second_end(e) + 1 << std::endl;
}

void AdjacencyListGraph::write_dimacs(std::ofstream& file)
{
    file << "p edge " << number_of_vertices() << " " << number_of_edges() << std::endl;
    for (EdgeId e = 0; e < number_of_edges(); ++e)
        file << "e " << first_end(e) + 1 << " " << second_end(e) + 1 << std::endl;
}
