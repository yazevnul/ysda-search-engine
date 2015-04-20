#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <cinttypes>


namespace ygraph {

    template <typename T>
    using AdjacencyList = std::vector<std::vector<T>>;


    template <typename T>
    struct Graph {
        std::uint32_t vertices_count = 0;
        std::uint32_t edges_count = 0;
        AdjacencyList<T> adjacency_list;
    };

}  // namespace ygraph


namespace ysave_load {

    /* vertices_count \s edges_count
     * out_degree \t vertice \s vertice ...
     * ...
     */
    template <typename T>
    void Save(const ygraph::Graph<T>& graph, const std::string& file_name) {
        auto&& output = std::ofstream{file_name};
        output << graph.vertices_count << ' ' << graph.edges_count << '\n';
        for (const auto& list_: graph.adjacency_list) {
            output << list_.size() << '\t';
            for (auto index = size_t{}; index < list_.size(); ++index) {
                if (0 != index) {
                    output << ' ';
                }
                output << list_[index];
            }
            output << '\n';
        }
    }


    template <typename T>
    ygraph::Graph<T> Load(const std::string& file_name) {
        auto&& input = std::ifstream{file_name};
        auto graph = ygraph::Graph<T>{};
        {
            auto line = std::string{};
            std::getline(input, line);
            auto&& parser = std::stringstream{line};
            parser >> graph.vertices_count >> graph.edges_count;
        }
        graph.adjacency_list.resize(graph.vertices_count);
        for (struct { std::string line; size_t index = 0; } loop; std::getline(input, loop.line);
                ++loop.index) {
            auto&& parser = std::stringstream{loop.line};
            auto out_degree = std::size_t{};
            parser >> out_degree;
            graph.adjacency_list[loop.index].resize(out_degree);
            for (const auto& vertice: graph.adjacency_list[loop.index]) {
                parser >> vertice;
            }
        }
        return graph;
    }


}  // namespace ysave_load
