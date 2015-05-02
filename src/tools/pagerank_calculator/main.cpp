#include <third_party/cxxopts/src/cxxopts.hpp>

#include <library/accurate_accumulate/kahan_accumulator.h>
#include <library/graph/graph.h>
#include <library/graph/statistics.h>

#include <fstream>
#include <future>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>

#include <cassert>
#include <cstddef>
#include <cstdint>


struct Args {
    std::string graph_file_name;
    std::string output_file_name;
    double dumping_factor = 0.5;
    double epsilon = 0.0001;
};


auto ParseOptions(int argc, char* argv[]) {
    auto options = cxxopts::Options{
        argv[0], std::string{"\n  "} + "Iteratively calculate PageRank for a given graph"
    };
    auto args = Args{};
    options.add_options()(
        "g,graph",
        "File with graph",
        cxxopts::value<>(args.graph_file_name),
        "FILE"
    )(
        "o,output",
        "Output file. Format: vertice \\t PageRank_value",
        cxxopts::value<>(args.output_file_name),
        "FILE"
    )(
        "d,dumping",
        "Dumping factor",
        cxxopts::value<>(args.dumping_factor),
        "FLOAT"
    )(
        "e,epsilon",
        "Stop when mean squared difference between iterations will be less then epsilon",
        cxxopts::value<>(args.epsilon),
        "FLOAT"
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


template <typename T>
T GetMeanSquaredDifference(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    assert(lhs.size() == rhs.size());
    if (lhs.empty()) {
        return {};
    }

    const auto size = lhs.size();
    auto sum = yaccumulate::KahanAccumulator<T>{};
    for (auto index = size_t{}; index < size; ++index) {
        sum += (lhs[index] - rhs[index]) * (lhs[index] - rhs[index]);
    }
    return sum.get() / size;
}


std::vector<std::vector<std::uint32_t>> GetInputVertices(
    const ygraph::Graph<std::uint32_t>& graph
) {
    auto input_vertices = std::vector<std::vector<std::uint32_t>>(graph.vertices_count);
    for (auto from = std::uint32_t{}; from < graph.vertices_count; ++from) {
        for (const auto to: graph.adjacency_list[from]) {
            input_vertices[to].push_back(from);
        }
    }

    return input_vertices;
}


std::vector<double> CalculatePageRank(
    const std::vector<std::uint32_t>& out_degree,
    const std::vector<std::vector<std::uint32_t>>& input_vertices,
    const double dumping_factor, const double epsilon
){
    assert(out_degree.size() == input_vertices.size());

    const auto size = out_degree.size();
    auto page_rank_prev = std::vector<double>(out_degree.size(), 1.);
    auto page_rank = std::vector<double>(out_degree.size(), 0.);
    auto msd = std::numeric_limits<double>::max();
    auto iteration = std::uint32_t{};
    for (;;) {
        std::clog << "iteration " << iteration << '\t' << "msd: " << msd << '\n';
        for (auto to = size_t{}; to < size; ++to) {
            auto sum = yaccumulate::KahanAccumulator<double>{};
            for (const auto from: input_vertices[to]) {
                assert(out_degree[from] > 0.);
                sum += page_rank_prev[from] / out_degree[from];
            }
            sum = sum.get() * dumping_factor;
            sum += 1.0 - dumping_factor;
            page_rank[to] = sum.get();
        }

        if (msd < epsilon) {
            break;
        }
        msd = GetMeanSquaredDifference(page_rank, page_rank_prev);
        page_rank.swap(page_rank_prev);
    }
    return page_rank;
}


void Print(const std::vector<double>& page_rank, std::ostream& output) {
    for (auto index = size_t{}; index < page_rank.size(); ++index) {
        output << index << '\t' << page_rank[index] << '\n';
    }
}


void Print(const std::vector<double>& page_rank, const std::string& output_file_name) {
    if ("-" == output_file_name) {
        Print(page_rank, std::cout);
    } else {
        auto&& output = std::ofstream{output_file_name};
        Print(page_rank, output);
    }
}


int main(int argc, char* argv[]) {
    const auto args = ParseOptions(argc, argv);
    const auto graph = ysave_load::Load<ygraph::Graph<std::uint32_t>>(args.graph_file_name);

    auto out_degree_future = std::async([&graph]{ return ygraph::GetOutDegree(graph); });
    auto input_vertices_future = std::async([&graph]{ return GetInputVertices(graph); });

    const auto page_rank = CalculatePageRank(
        out_degree_future.get(), input_vertices_future.get(),
        args.dumping_factor, args.epsilon
    );
    Print(page_rank, args.output_file_name);

    return EXIT_SUCCESS;
}

