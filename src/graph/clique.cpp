#include "optimizationtools/graph/clique.hpp"

//#include "optimizationtools/utils/common.hpp"
#include "optimizationtools/containers/indexed_set.hpp"
#include "optimizationtools/containers/indexed_4ary_heap.hpp"

//#include <iostream>

using namespace optimizationtools;

namespace
{

inline void add_vertex_to_clique(
        const AdjacencyListGraph& graph,
        std::vector<VertexId>& clique,
        const std::vector<uint8_t>* edges_is_forbidden,
        VertexId vertex_id,
        optimizationtools::IndexedSet& clique_candidates,
        optimizationtools::IndexedSet& edges_tmp)
{
    const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
    clique.push_back(vertex_id);
    edges_tmp.clear();
    if (edges_is_forbidden == nullptr) {
        for (const AdjacencyListGraph::VertexEdge& vertex_edge: vertex.edges)
            edges_tmp.add(vertex_edge.vertex_id);
    } else {
        for (const AdjacencyListGraph::VertexEdge& vertex_edge: vertex.edges)
            if ((*edges_is_forbidden)[vertex_edge.edge_id] == 0)
                edges_tmp.add(vertex_edge.vertex_id);
    }
    for (auto it = clique_candidates.begin(); it != clique_candidates.end();) {
        if (!edges_tmp.contains(*it)) {
            clique_candidates.remove(*it);
        } else {
            it++;
        }
    }
}

inline void fill_clique(
        const AdjacencyListGraph& graph,
        std::vector<VertexId>& clique,
        const std::vector<uint8_t>* edges_is_forbidden,
        optimizationtools::IndexedSet& clique_candidates,
        optimizationtools::IndexedSet& edges_tmp)
{
    while (!clique_candidates.empty()) {
        // Find the edges with the highest weight.
        VertexId vertex_best_id = -1;
        Weight value_best = 0;
        for (VertexId vertex_id: clique_candidates) {
            const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
            VertexId degree = 0;
            for (const AdjacencyListGraph::VertexEdge& vertex_edge: vertex.edges)
                if (clique_candidates.contains(vertex_edge.vertex_id))
                    degree++;
            //Weight value = vertex.weight;
            //Weight value = vertex.weight * vertex.edges.size();
            //Weight value = vertex.edges.size();
            //Weight value = degree;
            Weight value = vertex.weight * degree;
            if (vertex_best_id == -1
                    || value_best < value) {
                vertex_best_id = vertex_id;
                value_best = value;
            }
        }
        add_vertex_to_clique(
                graph,
                clique,
                edges_is_forbidden,
                vertex_best_id,
                clique_candidates,
                edges_tmp);
    }
}

}

std::vector<std::vector<VertexId>> optimizationtools::edge_clique_cover(
        const AdjacencyListGraph& graph)
{
    std::vector<std::vector<VertexId>> clique_cover;
    optimizationtools::IndexedSet clique_candidates(graph.number_of_vertices());
    optimizationtools::IndexedSet edges_tmp(graph.number_of_vertices());
    for (EdgeId edge_id = 0;
            edge_id < graph.number_of_edges();
            ++edge_id) {
        const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
        std::vector<VertexId> clique;
        clique_candidates.fill();
        add_vertex_to_clique(
                graph,
                clique,
                nullptr,
                edge.vertex_1_id,
                clique_candidates,
                edges_tmp);
        add_vertex_to_clique(
                graph,
                clique,
                nullptr,
                edge.vertex_2_id,
                clique_candidates,
                edges_tmp);
        fill_clique(
                graph,
                clique,
                nullptr,
                clique_candidates,
                edges_tmp);
        std::sort(clique.begin(), clique.end());
        clique_cover.push_back(clique);
    }

    // Remove duplicates.
    std::sort(clique_cover.begin(), clique_cover.end());
    clique_cover.erase(
            unique(clique_cover.begin(), clique_cover.end()),
            clique_cover.end());

    return clique_cover;
}

std::vector<std::vector<VertexId>> optimizationtools::vertex_clique_cover(
        const AdjacencyListGraph& graph)
{
    std::vector<std::vector<VertexId>> clique_cover;
    optimizationtools::IndexedSet clique_candidates(graph.number_of_vertices());
    optimizationtools::IndexedSet edges_tmp(graph.number_of_vertices());
    for (VertexId vertex_id = 0;
            vertex_id < graph.number_of_vertices();
            ++vertex_id) {
        std::vector<VertexId> clique;
        clique_candidates.fill();
        add_vertex_to_clique(
                graph,
                clique,
                nullptr,
                vertex_id,
                clique_candidates,
                edges_tmp);
        fill_clique(
                graph,
                clique,
                nullptr,
                clique_candidates,
                edges_tmp);
        std::sort(clique.begin(), clique.end());
        clique_cover.push_back(clique);
    }

    // Remove duplicates.
    std::sort(clique_cover.begin(), clique_cover.end());
    clique_cover.erase(
            unique(clique_cover.begin(), clique_cover.end()),
            clique_cover.end());

    return clique_cover;
}

std::vector<std::vector<VertexId>> optimizationtools::edge_clique_partition(
        const AdjacencyListGraph& graph)
{
    std::vector<std::vector<VertexId>> cliques;
    std::vector<EdgeId> sorted_edges(graph.number_of_edges());
    std::iota(sorted_edges.begin(), sorted_edges.end(), 0);
    std::sort(
            sorted_edges.begin(),
            sorted_edges.end(),
            [&graph](EdgeId edge_1_id, EdgeId edge_2_id) -> bool
            {
                const AdjacencyListGraph::Edge& edge_1 = graph.edge(edge_1_id);
                const AdjacencyListGraph::Edge& edge_2 = graph.edge(edge_2_id);
                const AdjacencyListGraph::Vertex& vertex_1_1 = graph.vertex(edge_1.vertex_1_id);
                const AdjacencyListGraph::Vertex& vertex_1_2 = graph.vertex(edge_1.vertex_2_id);
                const AdjacencyListGraph::Vertex& vertex_2_1 = graph.vertex(edge_2.vertex_1_id);
                const AdjacencyListGraph::Vertex& vertex_2_2 = graph.vertex(edge_2.vertex_2_id);

                Weight v1 = (vertex_1_1.weight + vertex_1_2.weight);
                Weight v2 = (vertex_2_1.weight + vertex_2_2.weight);
                //Weight v1 = (vertex_1_1.weight + vertex_1_2.weight) * (vertex_1_1.edges.size() + vertex_1_2.edges.size());
                //Weight v2 = (vertex_2_1.weight + vertex_2_2.weight) * (vertex_2_1.edges.size() + vertex_2_2.edges.size());
                //Weight v1 = (vertex_1_1.edges.size() + vertex_1_2.edges.size());
                //Weight v2 = (vertex_2_1.edges.size() + vertex_2_2.edges.size());
                return v1 > v2;
            });
    std::vector<uint8_t> edges_is_selected(graph.number_of_edges(), 0);
    optimizationtools::IndexedSet clique_candidates(graph.number_of_vertices());
    optimizationtools::IndexedSet edges_tmp(graph.number_of_vertices());
    for (EdgeId edge_pos = 0;
            edge_pos < graph.number_of_edges();
            ++edge_pos) {
        EdgeId edge_id = sorted_edges[edge_pos];
        if (edges_is_selected[edge_id])
            continue;
        const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);

        std::vector<VertexId> clique;
        clique_candidates.fill();
        add_vertex_to_clique(
                graph,
                clique,
                &edges_is_selected,
                edge.vertex_1_id,
                clique_candidates,
                edges_tmp);
        add_vertex_to_clique(
                graph,
                clique,
                &edges_is_selected,
                edge.vertex_2_id,
                clique_candidates,
                edges_tmp);
        fill_clique(
                graph,
                clique,
                &edges_is_selected,
                clique_candidates,
                edges_tmp);

        // Update edges_is_selected.
        clique_candidates.clear();
        for (VertexId vertex_id: clique)
            clique_candidates.add(vertex_id);
        for (VertexId vertex_id: clique) {
            const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
            for (const AdjacencyListGraph::VertexEdge& edge: vertex.edges)
                if (clique_candidates.contains(edge.vertex_id))
                    edges_is_selected[edge.edge_id] = 1;
        }

        cliques.push_back(clique);
    }

    return cliques;
}

std::vector<std::vector<VertexId>> optimizationtools::vertex_clique_partition_1(
        const AdjacencyListGraph& graph)
{
    std::vector<std::vector<VertexId>> cliques;
    std::vector<Weight> cliques_weights;

    optimizationtools::IndexedSet vertex_edges(graph.number_of_edges());
    for (VertexId vertex_id = 0;
            vertex_id < graph.number_of_vertices();
            ++vertex_id) {
        const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);

        vertex_edges.clear();
        for (const AdjacencyListGraph::VertexEdge& vertex_edge: vertex.edges)
            vertex_edges.add(vertex_edge.vertex_id);

        VertexId clique_id_best = -1;
        for (VertexId clique_id = 0;
                clique_id < (VertexId)cliques.size();
                ++clique_id) {
            const std::vector<VertexId>& clique = cliques[clique_id];
            Weight clique_weight = cliques_weights[clique_id];

            bool ok = true;
            for (VertexId clique_vertex_id: clique) {
                if (!vertex_edges.contains(clique_vertex_id)) {
                    ok = false;
                    break;
                }
            }
            if (!ok)
                continue;

            if (clique_id_best == -1
                    || cliques_weights[clique_id_best] < clique_weight) {
                clique_id_best = clique_id;
            }
        }

        if (clique_id_best != -1) {
            cliques[clique_id_best].push_back(vertex_id);
            cliques_weights[clique_id_best] += vertex.weight;
        } else {
            cliques.push_back({vertex_id});
            cliques_weights.push_back({vertex.weight});
        }
    }
    return cliques;
}

std::vector<std::vector<VertexId>> optimizationtools::vertex_clique_partition_2(
        const AdjacencyListGraph& graph)
{
    std::vector<std::vector<VertexId>> cliques;

    // For each edge, it's associated clique.
    std::vector<std::vector<VertexId>> edges_cliques(graph.number_of_edges());
    std::vector<Weight> edges_cliques_weights(graph.number_of_edges(), 0);

    // For each vertex, the list of edges cliques to which it belongs.
    // This is used to determine which edge clique to recompute when some clique
    // is selected.
    std::vector<std::vector<EdgeId>> vertices_edges_cliques(graph.number_of_vertices());
    optimizationtools::IndexedSet edges_cliques_to_update(graph.number_of_edges());

    // Vertices from the selected cliques.
    std::vector<uint8_t> vertices_is_selected(graph.number_of_vertices(), false);

    // Initialize cliques.
    optimizationtools::IndexedSet clique_candidates(graph.number_of_vertices());
    optimizationtools::IndexedSet edges_tmp(graph.number_of_vertices());
    for (EdgeId edge_id = 0;
            edge_id < graph.number_of_edges();
            ++edge_id) {
        const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
        std::vector<VertexId> clique;
        clique_candidates.fill();
        add_vertex_to_clique(
                graph,
                clique,
                nullptr,
                edge.vertex_1_id,
                clique_candidates,
                edges_tmp);
        add_vertex_to_clique(
                graph,
                clique,
                nullptr,
                edge.vertex_2_id,
                clique_candidates,
                edges_tmp);
        fill_clique(
                graph,
                clique,
                nullptr,
                clique_candidates,
                edges_tmp);

        edges_cliques_weights[edge_id] = 0;
        for (VertexId vertex_id: clique) {
            const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
            vertices_edges_cliques[vertex_id].push_back(edge_id);
            edges_cliques_weights[edge_id] += vertex.weight;
        }
        edges_cliques[edge_id] = clique;
    }

    auto f = [&edges_cliques_weights](EdgeId edge_id) { return -1.0 * edges_cliques_weights[edge_id]; };
    optimizationtools::Indexed4aryHeap<Weight> heap(graph.number_of_edges(), f);

    while (!heap.empty()) {
        // Find the best clique.
        auto p = heap.top();
        EdgeId edge_id = p.first;
        heap.pop();
        const std::vector<VertexId>& clique = edges_cliques[edge_id];
        cliques.push_back(clique);
        //std::cout << "Clique from edge " << p.first
        //    << " of size " << clique.size()
        //    << " and weight " << -p.second << std::endl;

        // Update selected vertices.
        for (VertexId vertex_id: clique)
            vertices_is_selected[vertex_id] = 1;

        // Find the cliques to update.
        edges_cliques_to_update.clear();
        for (VertexId vertex_id: clique)
            for (EdgeId edge_id: vertices_edges_cliques[vertex_id])
                edges_cliques_to_update.add(edge_id);
        // Update edges cliques.
        for (EdgeId edge_id: edges_cliques_to_update) {
            const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);

            // For each vertex of the clique, remove the clique from its list of
            // cliques.
            for (VertexId vertex_id: edges_cliques[edge_id]) {
                for (EdgeId pos = 0;
                        pos < (EdgeId)vertices_edges_cliques[vertex_id].size();
                        ++pos) {
                    if (vertices_edges_cliques[vertex_id][pos] == edge_id) {
                        vertices_edges_cliques[vertex_id][pos]
                            = vertices_edges_cliques[vertex_id].back();
                        vertices_edges_cliques[vertex_id].pop_back();
                    }
                }
            }

            // If one of the vertex of the edge is already selected, remove it
            // from the queue.
            if (vertices_is_selected[edge.vertex_1_id]
                    || vertices_is_selected[edge.vertex_2_id]) {
                heap.update_key(edge_id, -std::numeric_limits<Weight>::infinity());
                heap.pop();
                continue;
            }

            // Compute the new clique.
            std::vector<VertexId> clique;
            clique_candidates.fill();
            add_vertex_to_clique(
                    graph,
                    clique,
                    nullptr,
                    edge.vertex_1_id,
                    clique_candidates,
                    edges_tmp);
            add_vertex_to_clique(
                    graph,
                    clique,
                    nullptr,
                    edge.vertex_2_id,
                    clique_candidates,
                    edges_tmp);
            // Remove selected candidiates.
            for (auto it = clique_candidates.begin(); it != clique_candidates.end();) {
                if (vertices_is_selected[*it]) {
                    clique_candidates.remove(*it);
                } else {
                    it++;
                }
            }
            fill_clique(
                    graph,
                    clique,
                    nullptr,
                    clique_candidates,
                    edges_tmp);

            edges_cliques_weights[edge_id] = 0;
            for (VertexId vertex_id: clique) {
                const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
                vertices_edges_cliques[vertex_id].push_back(edge_id);
                edges_cliques_weights[edge_id] += vertex.weight;
            }
            edges_cliques[edge_id] = clique;
            heap.update_key(edge_id, -edges_cliques_weights[edge_id]);
        }
    }

    return cliques;
}
