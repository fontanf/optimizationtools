#include "optimizationtools/graph/clique_graph.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>

using namespace optimizationtools;

CliqueGraph::CliqueGraph(
        std::string instance_path,
        std::string format):
    neighbors_tmp_(0)
{
    std::ifstream file(instance_path);
    if (!file.good())
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");

    if (format == "cliquegraph") {
        read_cliquegraph(file);
    } else {
        throw std::invalid_argument(
                "Unknown instance format \"" + format + "\".");
    }
}

void CliqueGraph::read_cliquegraph(std::ifstream& file)
{
    std::string tmp;
    std::vector<std::string> line;
    CliqueId number_of_cliques = -1;
    VertexId n = -1;

    file >> tmp >> number_of_cliques >> tmp >> n;

    for (VertexId v = 0; v < n; ++v)
        add_vertex();

    for (CliqueId clique_id = 0; clique_id < number_of_cliques; ++clique_id) {
        getline(file, tmp);
        line = optimizationtools::split(tmp, ' ');
        std::vector<VertexId> clique;
        for (std::string element: line)
            clique.push_back(stol(element));
        add_clique(clique);
    }
}

VertexId CliqueGraph::add_vertex(Weight weight)
{
    Vertex vertex;
    vertex.id = vertices_.size();
    vertex.weight = weight;
    vertices_.push_back(vertex);
    total_weight_ += weight;
    neighbors_tmp_.add_element();
    return vertex.id;
}

CliqueId CliqueGraph::add_clique()
{
    CliqueId id = cliques_.size();
    cliques_.push_back({});
    return id;
}

CliqueId CliqueGraph::add_clique(
        const std::vector<VertexId>& clique)
{
    CliqueId id = cliques_.size();
    cliques_.push_back(clique);
    for (VertexId v: clique) {
        vertices_[v].cliques.push_back(id);
        vertices_[v].degree += clique.size() - 1;
        if (maximum_degree_ < degree(v))
            maximum_degree_ = degree(v);
    }
    number_of_edges_ += clique.size() * (clique.size() - 1) / 2;
    return id;
}

void CliqueGraph::add_vertex_to_clique(CliqueId clique_id, VertexId v)
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
