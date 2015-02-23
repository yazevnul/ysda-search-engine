#include <library/index/index.pb.h>
#include <library/index/index.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>


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


yindex::Index ParseIndex(std::istream& input) {
    yindex::Index index;

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


inline yindex::Index ParseIndex(const std::string& file_name) {
    std::ifstream input(file_name);
    return ParseIndex(input);
}


int main(int argc, char** argv) {
    const std::vector<std::string> args{argv + 1, argv + argc};

    assert(2 == args.size());

    const yindex::Index index = ParseIndex(args[0]);
    yindex::io::Save(index, args[1]);

    return 0;
}
