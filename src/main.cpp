#include "document.pb.h"
#include "word.pb.h"

#include "protobuf_helpers.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <exception>

#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


using DocumentId = google::protobuf::uint64;
using WordId = google::protobuf::uint64;

using Dictionary = std::unordered_map<std::string, yindex::WordInDictionary>;
using Index = std::unordered_map<WordId, yindex::WordInIndex>;


Dictionary LoadDictionary(std::istream& input) {
    Dictionary dictionary;

    std::string word;
    WordId word_id = 0;
    google::protobuf::uint64 frequency = 0;
    while (input >> word >> word_id >> frequency) {
        assert(dictionary.find(word) == dictionary.end());

        yindex::WordInDictionary entry;
        entry.set_word(word);
        entry.set_id(word_id);
        entry.set_frequency(frequency);

        dictionary[word] = entry;
    }

    return dictionary;
}


inline Dictionary LoadDictionary(const std::string& file_name) {
    std::ifstream input(file_name);
    return LoadDictionary(input);
}


void SaveDictionary(const Dictionary& dictionary, const std::string& file_name) {
    int fd = open(file_name.c_str(), O_WRONLY);
    if (!fd)
        throw std::runtime_error("open failed");

    try {
        google::protobuf::io::FileOutputStream output(fd);
        for (const auto& value: dictionary) {
            WriteDelimitedTo(value.second, &output);
        }

    } catch (const std::exception& exc) {
        if (!close(fd))
            throw std::runtime_error("close failed");

        throw exc;
    }

    if (!close(fd))
        throw std::runtime_error("close failed");
}


Index LoadIndex(std::istream& input) {
    Index index;

    for (std::string line; std::getline(input, line);) {
        std::stringstream line_parser(line);

        WordId word_id = 0;
        line_parser >> word_id;

        assert(index.find(word_id) == index.end());

        yindex::WordInIndex entry;
        DocumentId document_id = 0;
        google::protobuf::uint64 frequency = 0;
        for (char colon; line_parser >> document_id >> colon >> frequency;) {
            auto document_entry = entry.add_documents();
            document_entry->set_word_id(word_id);
            document_entry->set_document_id(document_id);
            document_entry->set_word_frequency(frequency);
        }

        index[word_id] = entry;
    }

    return index;
}


inline Index LoadIndex(const std::string& file_name) {
    std::ifstream input(file_name);
    return LoadIndex(input);
}


void SaveIndex(const Index& index, const std::string& file_name) {
    int fd = open(file_name.c_str(), O_WRONLY);
    if (!fd) {
        throw std::runtime_error("open failed");
    }

    try {
        google::protobuf::io::FileOutputStream output(fd);
        for (const auto& value: index) {
        }
    } catch (const std::exception& exc) {
        if (!close(fd)) {
            throw std::runtime_error("close failed");
        }

        throw exc;
    }

    if (!close(fd)) {
        throw std::runtime_error("close failed");
    }
}


int main(int argc, char** argv) {
    const std::vector<std::string> args{argv + 1, argv + argc};

    LoadDictionary(args[0]);
    LoadIndex(args[1]);

    return 0;
}
