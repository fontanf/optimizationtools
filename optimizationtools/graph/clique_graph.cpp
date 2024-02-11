#include "optimizationtools/graph/clique_graph.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <vector>
#include <fstream>
#include <iostream>

using namespace optimizationtools;

void CliqueGraphBuilder::read(
        const std::string& instance_path,
        const std::string& format)
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

void CliqueGraphBuilder::read_cliquegraph(std::ifstream& file)
{
    std::string tmp;
    std::vector<std::string> line;
    CliqueId number_of_cliques = -1;
    VertexId n = -1;

    file >> tmp >> number_of_cliques >> tmp >> n;

    for (VertexId vertex_id = 0; vertex_id < n; ++vertex_id)
        add_vertex();

    getline(file, tmp);
    for (CliqueId clique_id = 0; clique_id < number_of_cliques; ++clique_id) {
        getline(file, tmp);
        line = optimizationtools::split(tmp, ' ');
        std::vector<VertexId> clique;
        for (const std::string& element: line)
            clique.push_back(stol(element));
        add_clique(clique);
    }
}

VertexId CliqueGraphBuilder::add_vertex(Weight weight)
{
    VertexId vertex_id = graph_.vertices_.size();

    CliqueGraph::Vertex vertex;
    vertex.weight = weight;
    graph_.vertices_.push_back(vertex);
    return vertex_id;
}

CliqueId CliqueGraphBuilder::add_clique()
{
    CliqueId id = graph_.cliques_.size();
    graph_.cliques_.push_back({});
    return id;
}

CliqueId CliqueGraphBuilder::add_clique(
        const std::vector<VertexId>& clique)
{
    CliqueId id = graph_.cliques_.size();
    graph_.cliques_.push_back(clique);
    for (VertexId vertex_id: clique) {
        graph_.vertices_[vertex_id].cliques.push_back(id);
        graph_.vertices_[vertex_id].degree += clique.size() - 1;
    }
    graph_.number_of_edges_ += clique.size() * (clique.size() - 1) / 2;
    return id;
}

void CliqueGraphBuilder::add_vertex_to_clique(
        CliqueId clique_id,
        VertexId vertex_id)
{
    graph_.number_of_edges_ += graph_.cliques_[clique_id].size();
    for (VertexId vertex_id_2: graph_.cliques_[clique_id])
        graph_.vertices_[vertex_id_2].degree++;
    graph_.vertices_[vertex_id].degree += graph_.cliques_[clique_id].size();
    graph_.vertices_[vertex_id].cliques.push_back(clique_id);
    graph_.cliques_[clique_id].push_back(vertex_id);
}

void CliqueGraphBuilder::set_weight(
        VertexId vertex_id,
        Weight weight)
{
    graph_.vertices_[vertex_id].weight = weight;
};
