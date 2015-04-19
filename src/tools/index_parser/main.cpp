#include <third_party/cxxopts/src/cxxopts.hpp>

#include <library/accurate_accumulate/kahan_accumulator.h>
#include <library/index/index.h>
#include <library/protobuf_helpers/serialization.h>
#include <library/timer/raii_printing_timer.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>


struct DocumentIdWithWordFrequency {
    yindex::DocumentId document_id = 0;
    uint64_t word_frequency = 0;
};


inline bool operator <(
    const DocumentIdWithWordFrequency& lhs, const DocumentIdWithWordFrequency& rhs
){
    return std::tie(lhs.document_id, lhs.word_frequency)
           < std::tie(rhs.document_id, rhs.word_frequency);
}


yindex::InvertedIndex ParseIndex(std::istream& input) {
    yindex::InvertedIndex index;

    std::vector<DocumentIdWithWordFrequency> documentIdsWithWordFrequencies;
    yindex::AssociatedDocuments entry;
    for (std::string line; std::getline(input, line);) {
        entry.Clear();
        documentIdsWithWordFrequencies.clear();
        std::stringstream line_parser(line);

        yindex::WordId word_id = 0;
        line_parser >> word_id;

        assert(index.find(word_id) == index.end());

        DocumentIdWithWordFrequency documentIdWithWordFrequency;
        for (char colon;
                line_parser >> documentIdWithWordFrequency.document_id
                            >> colon
                            >> documentIdWithWordFrequency.word_frequency;) {
            assert(':' == colon);
            documentIdsWithWordFrequencies.push_back(documentIdWithWordFrequency);
        }

        std::sort(documentIdsWithWordFrequencies.begin(), documentIdsWithWordFrequencies.end());
        for (const auto& value: documentIdsWithWordFrequencies) {
            auto document_entry = entry.add_documents();
            document_entry->set_document_id(value.document_id);
            document_entry->set_word_frequency(value.word_frequency);
        }

        index[word_id] = entry;
    }

    return index;
}


inline yindex::InvertedIndex ParseIndex(const std::string& file_name) {
    std::ifstream input(file_name);
    return ParseIndex(input);
}


yindex::IndexStatistics AddStatistics(yindex::InvertedIndex& index) {
    std::unordered_map<yindex::DocumentId, uint64_t> document_to_length;
    for (const auto& value: index) {
        for (int jindex = 0; jindex < value.second.documents_size(); ++jindex) {
            document_to_length[value.second.documents(jindex).document_id()]
                += value.second.documents(jindex).word_frequency();
        }
    }

    yaccumulate::KahanAccumulator<double> average_document_length_calculator;
    for (const auto& value: document_to_length) {
        average_document_length_calculator += static_cast<double>(value.second)
                                              / document_to_length.size();
    }

    constexpr double EPSILON = 1e-6;
    const double average_document_legnth = fabs(average_document_length_calculator.get()) <= EPSILON
        ? 1
        : average_document_length_calculator.get();
    for (auto& value: index) {
        for (int jindex = 0; jindex < value.second.documents_size(); ++jindex) {
            value.second.mutable_documents(jindex)->set_dl_by_avg_dl(
                document_to_length[value.second.documents(jindex).document_id()]
                / average_document_legnth
            );
        }
    }

    yindex::IndexStatistics statistics;
    statistics.set_number_of_documents(document_to_length.size());
    statistics.set_average_document_length(average_document_legnth);

    return statistics;
}


struct Args {
    std::string input_file_name;
    std::string output_file_name;
    std::string statistics_file_name;
};


auto ParseOptions(int argc, char* argv[]) {
    auto options = cxxopts::Options{argv[0], "\n  Parse index"};
    auto args = Args{};
    options.add_options()(
        "i,input",
        "Index. Expected format: word_id \\t doc_id:count \\t ...",
        cxxopts::value<>(args.input_file_name),
        "FILE"
    )(
        "o,output",
        "Parsed index",
        cxxopts::value<>(args.output_file_name),
        "FILE"
    )(
        "s,statistics",
        "Index stattistics",
        cxxopts::value<>(args.statistics_file_name),
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


int main(int argc, char** argv) {
    const auto args = ParseOptions(argc, argv);

    yindex::InvertedIndex index;
    {
        ytimer::RaiiPrintingTimer timer("Parsing index");
        index = ParseIndex(args.input_file_name);
    }

    yindex::IndexStatistics index_statistics;
    {
        ytimer::RaiiPrintingTimer timer("Calculating statistics");
        index_statistics = AddStatistics(index);
    }

    {
        ytimer::RaiiPrintingTimer timer("Saving to file");
        yindex::io::Save(index, args.output_file_name);
        yproto::WriteDelimitedToFile(index_statistics, args.statistics_file_name);
    }

    return EXIT_SUCCESS;
}
