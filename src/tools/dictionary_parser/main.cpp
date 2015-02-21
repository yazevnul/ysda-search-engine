#include <library/index/index.h>
#include <library/index/dictionary.pb.h>

#include <cassert>
#include <cstdint>
#include <fstream>
#include <istream>
#include <string>
#include <vector>


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
    std::ifstream input(file_name);
    return ParseDictionary(input);
}


int main(int argc, char** argv) {
    const std::vector<std::string> args{argv + 1, argv + argc};

    assert(args.size() == 2);

    const yindex::Dictionary dictionary = ParseDictionary(args[0]);
    yindex::io::Save(dictionary, args[1]);

    return 0;
}
