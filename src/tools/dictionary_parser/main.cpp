#include <third_party/cxxopts/src/cxxopts.hpp>

#include <library/index/index.h>
#include <library/timer/raii_printing_timer.h>

#include <fstream>
#include <istream>
#include <string>
#include <vector>

#include <cassert>
#include <cstdint>

struct Args {
    std::string input_file_name;
    std::string output_file_name;
};


auto ParseOptions(int argc, char* argv[]) {
    auto options = cxxopts::Options{argv[0], "\n  Parse dictionary"};
    auto args = Args{};
    options.add_options()(
        "i,input",
        "Dictionary. Expected format: word_id \\t word_frequency",
        cxxopts::value<std::string>(args.input_file_name),
        "FILE"
    )(
        "o,output",
        "Parsed dictionary.",
        cxxopts::value<std::string>(args.output_file_name),
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


yindex::Dictionary ParseDictionary(std::istream& input) {
    yindex::Dictionary dictionary;

    std::string word;
    yindex::WordId word_id = 0;
    uint64_t word_frequency = 0;
    while (input >> word >> word_id >> word_frequency) {
        assert(dictionary.find(word) == dictionary.end());

        yindex::DictionaryEntry entry;
        entry.set_word_id(word_id);
        entry.set_word_frequency(word_frequency);

        dictionary[word] = entry;
    }

    return dictionary;
}


inline yindex::Dictionary ParseDictionary(const std::string& file_name) {
    auto&& input = std::ifstream{file_name};
    return ParseDictionary(input);
}


int main(int argc, char** argv) {
    const auto args = ParseOptions(argc, argv);

    yindex::Dictionary dictionary;
    {
        ytimer::RaiiPrintingTimer timer("Parsing dictionary");
        dictionary = ParseDictionary(args.input_file_name);
    }
    {
        ytimer::RaiiPrintingTimer timer("Saving dictionary");
        yindex::io::Save(dictionary, args.output_file_name);
    }

    return EXIT_SUCCESS;
}

