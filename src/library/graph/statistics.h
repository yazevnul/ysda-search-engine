#pragma once

#include "graph.h"

#include <vector>

#include <cstddef>
#include <cstdint>


namespace ygraph {

    template <typename T>
    std::vector<std::uint32_t> GetInDegree(const Graph<T>& graph);

    template <typename T>
    std::vector<std::uint32_t> GetOutDegree(const Graph<T>& graph);

}  // namesapce ygraph


// Implementation BEGI

template <typename T>
std::vector<std::uint32_t> ygraph::GetInDegree(const Graph<T>& graph) {
    auto in_degree = std::vector<std::uint32_t>(graph.vertices_count);
    for (auto from_vertice = size_t{}; from_vertice < graph.adjacency_list.size(); ++from_vertice) {
        for (const auto to_vertice: graph.adjacency_list[from_vertice]) {
            ++in_degree[to_vertice];
        }
    }
    return in_degree;
}


template <typename T>
std::vector<std::uint32_t> ygraph::GetOutDegree(const Graph<T>& graph) {
    auto out_degree = std::vector<std::uint32_t>(graph.vertices_count);
    for (auto from_vertice = size_t{}; from_vertice < graph.adjacency_list.size(); ++from_vertice) {
        out_degree[from_vertice] = graph.adjacency_list[from_vertice].size();
    }
    return out_degree;
}

// Implementation END
