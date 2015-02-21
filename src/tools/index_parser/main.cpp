#include <library/index/index.pb.h>
#include <library/index/index.h>

#include <cassert>
#include <cstdint>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>


yindex::Index ParseIndex(std::istream& input) {
    yindex::Index index;

    yindex::AssociatedDocuments entry;
    for (std::string line; std::getline(input, line);) {
        entry.Clear();
        std::stringstream line_parser(line);

        yindex::WordId word_id = 0;
        line_parser >> word_id;

        assert(index.find(word_id) == index.end());

        yindex::DocumentId document_id = 0;
        uint64_t frequency = 0;
        for (char colon; line_parser >> document_id >> colon >> frequency;) {
            auto document_entry = entry.add_documents();
            document_entry->set_document_id(document_id);
            document_entry->set_word_frequency(frequency);
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
