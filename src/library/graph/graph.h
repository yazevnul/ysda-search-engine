#pragma once

#include <library/save_load/save_load.h>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <cinttypes>


namespace ygraph {

    template <typename T>
    using AdjacencyList = std::vector<std::vector<T>>;


    template <typename T>
    struct Graph {
        std::uint32_t vertices_count = 0;
        std::uint32_t edges_count = 0;
        std::uint32_t sink_vertices_count = 0;  // number of vertices with 0 out degree
        AdjacencyList<T> adjacency_list;
    };


    template <typename T>
    using SparceAdjacencyList = std::unordered_map<T, std::vector<T>>;


    template <typename T>
    struct SparceGraph {
        std::uint32_t vertices_count = 0;
        std::uint32_t edges_count = 0;
        std::uint32_t sink_vertices_count = 0;  // number of vertices with 0 out degree
        SparceAdjacencyList<T> adjacency_list;
    };


}  // namespace ygraph


namespace ysave_load {

    /* vertices_count \s edges_count \s sink_vertices_count
     * vertice \t out_degree \t vertice \s vertice ...
     * ...
     */
    template <typename T>
    void Save(const ygraph::Graph<T>& graph, const std::string& file_name) {
        auto&& output = std::ofstream{file_name};
        output << graph.vertices_count
               << ' ' << graph.edges_count
               << ' ' << graph.sink_vertices_count
               << '\n';
        for (auto index = size_t{}; index < graph.adjacency_list.size(); ++index) {
            const auto& vertices_list = graph.adjacency_list[index];
            output << index << '\t' << vertices_list.size() << '\t';
            for (auto jindex = size_t{}; jindex < vertices_list.size(); ++jindex) {
                if (0 != jindex) {
                    output << ' ';
                }
                output << vertices_list[jindex];
            }
            output << '\n';
        }
    }


    namespace detail {
        template <typename T>
        struct LoadImpl<ygraph::Graph<T>> {
            static ygraph::Graph<T> Do(const std::string& file_name) {
                auto&& input = std::ifstream{file_name};
                auto graph = ygraph::Graph<T>{};
                {
                    auto line = std::string{};
                    std::getline(input, line);
                    auto&& parser = std::stringstream{line};
                    parser >> graph.vertices_count
                           >> graph.edges_count
                           >> graph.sink_vertices_count;
                }
                graph.adjacency_list.resize(graph.vertices_count);
                for (auto line = std::string{}; std::getline(input, line);) {
                    auto&& parser = std::stringstream{line};
                    auto vertice = T{};
                    auto out_degree = std::size_t{};
                    parser >> vertice >> out_degree;
                    graph.adjacency_list[vertice].resize(out_degree);
                    for (auto& to: graph.adjacency_list[vertice]) {
                        parser >> to;
                    }
                }
                return graph;
            }
        };
    };


    template <typename T>
    void Save(const ygraph::SparceGraph<T>& graph, const std::string& file_name) {
        auto&& output = std::ofstream{file_name};
        output << graph.vertices_count
               << ' ' << graph.edges_count
               << ' ' << graph.sink_vertices_count
               << '\n';
        for (const auto& vertice_info: graph.adjacency_list) {
            output << vertice_info.first << '\t' << vertice_info.second.size() << '\t';
            for (auto jindex = size_t{}; jindex < vertice_info.second.size(); ++jindex) {
                if (0 != jindex) {
                    output << ' ';
                }
                output << vertice_info.second[jindex];
            }
            output << '\n';
        }
    }


    namespace detail {
        template <typename T>
        struct LoadImpl<ygraph::SparceGraph<T>> {
            static ygraph::SparceGraph<T> Do(const std::string& file_name) {
                auto&& input = std::ifstream{file_name};
                auto graph = ygraph::SparceGraph<T>{};
                {
                    auto line = std::string{};
                    std::getline(input, line);
                    auto&& parser = std::stringstream{line};
                    parser >> graph.vertices_count
                           >> graph.edges_count
                           >> graph.sink_vertices_count;
                }
                for (auto line = std::string{}; std::getline(input, line);) {
                    auto&& parser = std::stringstream{line};
                    auto vertice = T{};
                    auto out_degree = std::size_t{};
                    parser >> vertice >> out_degree;
                    graph.adjacency_list[vertice].resize(out_degree);
                    for (auto& to: graph.adjacency_list[vertice]) {
                        parser >> to;
                    }
                }
                return graph;
            }
        };
    };

}  // namespace ysave_load

