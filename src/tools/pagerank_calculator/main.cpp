#include <third_party/cxxopts/src/cxxopts.hpp>

#include <iostream>
#include <string>

#include <cstddef>


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

int main(int argc, char* argv[]) {
    const auto args = ParseOptions(argc, argv);
    return EXIT_SUCCESS;
}
