#include "index.h"
#include "index.pb.h"
#include "dictionary.pb.h"

#include <library/protobuf_helpers/serialization.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <cassert>
#include <exception>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void yindex::io::Save(const yindex::Dictionary& dictionary, const std::string& file_name) {
    int fd = open(file_name.c_str(), O_WRONLY | O_CREAT, 0666);
    if (-1 == fd)
        throw std::runtime_error("open failed");

    try {
        google::protobuf::io::FileOutputStream output(fd);

        for (auto value: dictionary) {
            value.second.mutable_word()->set_word(value.first);
            WriteDelimitedTo(value.second, &output);
        }

    } catch (const std::exception& exc) {
        if (-1 == close(fd))
            throw std::runtime_error("close failed");

        throw exc;
    }

    if (-1 == close(fd))
        throw std::runtime_error("close failed");
}


yindex::Dictionary yindex::io::LoadDictionary(const std::string& file_name) {
    int fd = open(file_name.c_str(), O_RDONLY);
    if (-1 == fd)
        throw std::runtime_error("open failed");

    google::protobuf::io::FileInputStream input(fd);

    yindex::Dictionary dictionary;
    for (struct {yindex::DictionaryEntry value; std::string word; } loop;
         ReadDelimitedFrom(&input, &loop.value);)
    {
        loop.word = loop.value.word().word();
        assert(dictionary.find(loop.word) == dictionary.end());

        loop.value.clear_word();
        dictionary[loop.word] = loop.value;
    }

    if (-1 == close(fd))
        throw std::runtime_error("close failed");

    return dictionary;
}


void yindex::io::Save(const yindex::InvertedIndex& index, const std::string& file_name) {
    int fd = open(file_name.c_str(), O_WRONLY | O_CREAT, 0666);
    if (-1 == fd) {
        throw std::runtime_error("open failed");
    }

    try {
        google::protobuf::io::FileOutputStream output(fd);

        yindex::WordInIndex dummy_word;
        for (const auto& value: index) {
            dummy_word.set_word_id(value.first);

            WriteDelimitedTo(dummy_word, &output);
            WriteDelimitedTo(value.second, &output);
        }
    } catch (const std::exception& exc) {
        if (-1 == close(fd))
            throw std::runtime_error("close failed");

        throw exc;
    }

    if (-1 == close(fd))
        throw std::runtime_error("close failed");
}


yindex::InvertedIndex yindex::io::LoadIndex(const std::string& file_name) {
    int fd = open(file_name.c_str(), O_RDONLY);
    if (-1 == fd)
        throw std::runtime_error("open failed");

    google::protobuf::io::FileInputStream input(fd);

    yindex::InvertedIndex index;
    for (yindex::WordInIndex dummy_word; ReadDelimitedFrom(&input, &dummy_word);) {
        assert(index.find(dummy_word.word_id()) == index.end());

        if (!ReadDelimitedFrom(&input, &index[dummy_word.word_id()]))
            throw std::runtime_error("failed to read associated documents");
    }

    if (-1 == close(fd))
        throw std::runtime_error("close failed");

    return index;
}
