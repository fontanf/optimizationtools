#include "optimizationtools/graph/bipartite_graph.hpp"

#include "optimizationtools/utils/common.hpp"

//#include <iostream>

using namespace optimizationtools;

std::vector<uint8_t> optimizationtools::bipartite_graph_identify(
        const AdjacencyListGraph& graph)
{
    if (graph.number_of_vertices() == 0)
        return {};
    std::vector<uint8_t> vertices_sides(graph.number_of_vertices(), 2);
    VertexId vertex_0_id = 0;
    for (;;) {
        while (vertex_0_id < graph.number_of_vertices()
                && vertices_sides[vertex_0_id] != 2) {
            vertex_0_id++;
        }
        if (vertex_0_id == graph.number_of_vertices())
            break;

        std::vector<VertexId> queue = {vertex_0_id};
        vertices_sides[vertex_0_id] = 0;
        while (!queue.empty()) {
            VertexId vertex_id = queue.back();
            const AdjacencyListGraph::Vertex vertex = graph.vertex(vertex_id);
            queue.pop_back();
            for (const AdjacencyListGraph::VertexEdge& edge: vertex.edges) {
                if (vertices_sides[edge.vertex_id] == vertices_sides[vertex_id])
                    return {};
                if (vertices_sides[edge.vertex_id] != 2)
                    continue;
                vertices_sides[edge.vertex_id] = 1 - vertices_sides[vertex_id];
                queue.push_back(edge.vertex_id);
            }
        }
    }
    return vertices_sides;
}

std::vector<uint8_t> optimizationtools::bipartite_graph_maximum_matching(
        const AdjacencyListGraph& graph)
{
    //std::cout << "bipartite_graph_maximum_matching " << graph.number_of_vertices() << " " << graph.number_of_edges() << std::endl;

    std::vector<uint8_t> vertices_sides = bipartite_graph_identify(graph);

    // Find an initial maximal matching.
    std::vector<VertexId> vertices_matched_edge(graph.number_of_vertices(), -1);
    std::vector<uint8_t> edges_matched(graph.number_of_edges(), 0);
    EdgeId matching_size = 0;
    for (EdgeId edge_id = 0; edge_id < graph.number_of_edges(); ++edge_id) {
        const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
        if (vertices_matched_edge[edge.vertex_1_id] != -1)
            continue;
        if (vertices_matched_edge[edge.vertex_2_id] != -1)
            continue;
        edges_matched[edge_id] = 1;
        vertices_matched_edge[edge.vertex_1_id] = edge_id;
        vertices_matched_edge[edge.vertex_2_id] = edge_id;
        matching_size++;
    }
    //std::cout << "matching_size " << matching_size << std::endl;
    if (matching_size > graph.number_of_edges()) {
        throw std::logic_error(FUNC_SIGNATURE);
    }
    if (matching_size == graph.number_of_vertices() / 2)
        return edges_matched;

    std::vector<VertexId> bfs_queue(graph.number_of_vertices(), -1);
    std::vector<VertexId> dist(graph.number_of_vertices(), graph.number_of_vertices() + 1);
    std::vector<EdgeId> pred(graph.number_of_vertices(), -1);
    std::vector<VertexId> unmatched_leaves;
    std::vector<VertexId> path;
    std::vector<uint8_t> vertices_processed(graph.number_of_vertices(), 0);
    for (;;) {
        //std::cout << "it matching_size " << matching_size << std::endl;
        // Breadth first search.
        // Find all unmatched vertices in i.
        std::fill(dist.begin(), dist.end(), graph.number_of_vertices() + 1);
        unmatched_leaves.clear();
        VertexId queue_push_pos = 0;
        for (VertexId vertex_id = 0;
                vertex_id < graph.number_of_vertices();
                ++vertex_id) {
            if (vertices_sides[vertex_id] == 1)
                continue;
            if (vertices_matched_edge[vertex_id] == -1) {
                dist[vertex_id] = 0;
                bfs_queue[queue_push_pos] = vertex_id;
                queue_push_pos++;
            }
        }
        for (VertexId queue_pop_pos = 0;
                queue_pop_pos < queue_push_pos;
                ++queue_pop_pos) {
            VertexId vertex_id = bfs_queue[queue_pop_pos];
            const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
            bool has_child = false;
            for (const AdjacencyListGraph::VertexEdge& edge: vertex.edges) {
                if (dist[vertex_id] % 2 == 0) {
                    if (edges_matched[edge.edge_id] == 1)
                        continue;
                } else {
                    if (edges_matched[edge.edge_id] == 0)
                        continue;
                }
                if (dist[edge.vertex_id] <= dist[vertex_id] + 1)
                    continue;
                dist[edge.vertex_id] = dist[vertex_id] + 1;
                pred[edge.vertex_id] = edge.edge_id;
                bfs_queue[queue_push_pos] = edge.vertex_id;
                queue_push_pos++;
                has_child = true;
            }
            if (vertices_matched_edge[vertex_id] == -1
                    && dist[vertex_id] > 0
                    && !has_child) {
                unmatched_leaves.push_back(vertex_id);
            }
        }

        //std::cout << "retrieve paths..." << std::endl;
        std::fill(vertices_processed.begin(), vertices_processed.end(), 0);
        bool found = false;
        for (VertexId leaf_vertex_id: unmatched_leaves) {
            bool ok = true;
            path.clear();
            VertexId vertex_id = leaf_vertex_id;
            while (pred[vertex_id] != -1) {
                EdgeId edge_id = pred[vertex_id];
                const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
                path.push_back(edge_id);
                vertex_id = (edge.vertex_1_id == vertex_id)?
                    edge.vertex_2_id:
                    edge.vertex_1_id;
                if (vertices_processed[vertex_id]) {
                    ok = false;
                    break;
                }
            }
            if (!ok)
                continue;

            //std::cout << "path";
            //for (EdgeId edge_id: path) {
            //    const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
            //    std::cout << " " << edge_id
            //        << "," << edge.vertex_1_id
            //        << "," << edge.vertex_2_id;
            //}
            //std::cout << std::endl;

            // Apply path.
            for (EdgeId edge_id: path) {
                const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
                if (edges_matched[edge_id] == 1) {
                    edges_matched[edge_id] = 0;
                } else {
                    edges_matched[edge_id] = 1;
                    vertices_matched_edge[edge.vertex_1_id] = edge_id;
                    vertices_matched_edge[edge.vertex_2_id] = edge_id;
                }
                vertices_processed[edge.vertex_1_id] = 1;
                vertices_processed[edge.vertex_2_id] = 1;
            }
            matching_size++;
            if (matching_size > graph.number_of_edges()) {
                throw std::logic_error(FUNC_SIGNATURE);
            }
            found = true;
        }
        if (!found)
            break;
    }

    for (VertexId vertex_id = 0;
            vertex_id < graph.number_of_vertices();
            ++vertex_id) {
        const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
        EdgeId m = 0;
        for (const AdjacencyListGraph::VertexEdge& edge: vertex.edges)
            if (edges_matched[edge.edge_id] == 1)
                m++;
        if (m >= 2) {
            throw std::logic_error(
                    FUNC_SIGNATURE + ": "
                    "vertex " + std::to_string(vertex_id) + " is matched " + std::to_string(m) + " times.");
        }
    }

    return edges_matched;
}

std::vector<uint8_t> optimizationtools::bipartite_graph_minimum_cover(
        const AdjacencyListGraph& graph)
{
    std::vector<uint8_t> maximum_matching = bipartite_graph_maximum_matching(graph);
    std::vector<uint8_t> vertices_sides = bipartite_graph_identify(graph);

    // Build mate array: for each vertex, store its matched mate_id or -1
    std::vector<VertexId> mate(graph.number_of_vertices(), -1);
    for (EdgeId edge_id = 0; edge_id < graph.number_of_edges(); ++edge_id) {
        if (maximum_matching[edge_id] == 1) {
            const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
            mate[edge.vertex_1_id] = edge.vertex_2_id;
            mate[edge.vertex_2_id] = edge.vertex_1_id;
        }
    }

    // BFS from unmatched left vertices
    std::vector<uint8_t> visited(graph.number_of_vertices(), 0);
    std::vector<VertexId> queue;
    for (VertexId vertex_id = 0;
            vertex_id < graph.number_of_vertices();
            ++vertex_id) {
        if (vertices_sides[vertex_id] == 0 && mate[vertex_id] == -1) {
            visited[vertex_id] = 1;
            queue.push_back(vertex_id);
        }
    }

    for (VertexId queue_pos = 0; queue_pos < (VertexId)queue.size(); ++queue_pos) {
        VertexId vertex_id = queue[queue_pos];
        VertexId mate_id = mate[vertex_id];
        if (vertices_sides[vertex_id] == 0) {
            // vertex in L: traverse only non-matching edges to R
            const AdjacencyListGraph::Vertex& vertex = graph.vertex(vertex_id);
            for (const AdjacencyListGraph::VertexEdge& edge: vertex.edges) {
                if (mate_id == edge.vertex_id)
                    continue;
                if (visited[edge.vertex_id])
                    continue;
                visited[edge.vertex_id] = 1;
                queue.push_back(edge.vertex_id);
            }
        } else {
            // vertex in R: traverse only the matching edge back to L
            if (mate_id != -1 && !visited[mate_id]) {
                visited[mate_id] = 1;
                queue.push_back(mate_id);
            }
        }
    }

    // Build cover: (L \ Z) ∪ (R ∩ Z)
    std::vector<uint8_t> cover(graph.number_of_vertices(), 0);
    for (EdgeId edge_id = 0;
            edge_id < graph.number_of_edges();
            ++edge_id) {
        const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
        if (maximum_matching[edge_id] == 0)
            continue;
        if (vertices_sides[edge.vertex_2_id] == 1) {
            if (visited[edge.vertex_2_id]) {
                cover[edge.vertex_2_id] = 1;
            } else {
                cover[edge.vertex_1_id] = 1;
            }
        } else {
            if (visited[edge.vertex_1_id]) {
                cover[edge.vertex_1_id] = 1;
            } else {
                cover[edge.vertex_2_id] = 1;
            }
        }
    }

    // Check vertex cover.
    EdgeId maximum_matching_size = 0;
    for (EdgeId edge_id = 0;
            edge_id < graph.number_of_edges();
            ++edge_id) {
        const AdjacencyListGraph::Edge& edge = graph.edge(edge_id);
        if (cover[edge.vertex_1_id] == 0
                && cover[edge.vertex_2_id] == 0) {
            throw std::logic_error(
                    FUNC_SIGNATURE + ": "
                    "edge " + std::to_string(edge_id) + " is not covered.");
        }
        if (maximum_matching[edge_id] == 1)
            maximum_matching_size++;
    }
    VertexId vertex_cover_size = 0;
    for (VertexId vertex_id = 0;
            vertex_id < graph.number_of_vertices();
            ++vertex_id) {
        if (cover[vertex_id] == 1)
            vertex_cover_size++;
    }
    if (vertex_cover_size != maximum_matching_size) {
        throw std::logic_error(
                FUNC_SIGNATURE + ": "
                "vertex cover size is not equal to maximum matching size; "
                "vertex_cover_size: " + std::to_string(vertex_cover_size) + "; "
                "maximum_matching_size: " + std::to_string(maximum_matching_size) + "; ");
    }

    return cover;
}
