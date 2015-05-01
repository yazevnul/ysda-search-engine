#include <third_party/cxxopts/src/cxxopts.hpp>

#include <library/graph/algorithm.h>
#include <library/graph/graph.h>
#include <library/graph/statistics.h>

#include <fstream>
#include <future>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <cassert>
#include <cstdlib>


struct Args {
    std::string graph_file_name;
    std::uint32_t graph_center = {};
    std::string output_file_name;
};


Args ParseOptions(int argc, char* argv[]) {
    auto options = cxxopts::Options{argv[0], "\n  Calculate statistics for given graph"};
    auto args = Args{};
    options.add_options()(
        "g,graph",
        "File with graph",
        cxxopts::value<>(args.graph_file_name),
        "FILE"
    )(
        "c,center",
        "Graph center",
        cxxopts::value<>(args.graph_center),
        "INT"
    )(
        "o,output",
        "Where to save statistics",
        cxxopts::value<>(args.output_file_name),
        "FILE"
    )(
        "h,help",
        "Print help"
    );
    options.parse(argc, argv);
    if (options.count("help")) {
        std::cout << options.help({""}) << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    return args;
}


void PrintStatistics(
    const std::vector<std::uint32_t>& in_degree, const std::vector<std::uint32_t>& out_degree,
    const std::vector<std::uint32_t>& distance_from_center,
    std::ostream& output
){
    assert(in_degree.size() == out_degree.size());
    assert(in_degree.size() == distance_from_center.size());

    const auto vertices_count = in_degree.size();
    for (auto vertice = size_t{}; vertice < vertices_count; ++vertice) {
        output << vertice
               << '\t' << in_degree[vertice]
               << '\t' << out_degree[vertice]
               << '\t' << distance_from_center[vertice]
               << '\n';
    }
}


void PrintStatistics(
    const std::vector<std::uint32_t>& in_degree, const std::vector<std::uint32_t>& out_degree,
    const std::vector<std::uint32_t>& distance_from_center,
    const std::string output_file_name
){
    auto&& output = std::ofstream{output_file_name};
    PrintStatistics(in_degree, out_degree, distance_from_center, output);
}


template <typename T>
auto GetDistanceFromCenter(const ygraph::Graph<T>& graph, const T center) {
    const auto callback = std::make_unique<ygraph::BFSDistanceCallback<T>>(
        graph.vertices_count, center
    );
    ygraph::BFS(graph, center, *callback);
    return callback->Get();
}


int main(int argc, char* argv[]) {
    const auto args = ParseOptions(argc, argv);
    const auto graph = ysave_load::Load<ygraph::Graph<std::uint32_t>>(args.graph_file_name);

    auto in_degree_future = std::async(ygraph::GetInDegree<std::uint32_t>, std::ref(graph));
    auto out_degree_future = std::async(ygraph::GetOutDegree<std::uint32_t>, std::ref(graph));
    auto distance_from_center_future = std::async(
        GetDistanceFromCenter<std::uint32_t>, std::ref(graph), args.graph_center
    );
    PrintStatistics(
        in_degree_future.get(), out_degree_future.get(), distance_from_center_future.get(),
        args.output_file_name
    );

    return EXIT_SUCCESS;
}
