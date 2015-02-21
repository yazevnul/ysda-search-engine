#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>


namespace yindex {

    class DictionaryEntry;
    class AssociatedDocuments;

}  // namespace yindex


namespace yindex {

    using DocumentId = uint64_t;
    using WordId = uint64_t;

    using Dictionary = std::unordered_map<std::string, DictionaryEntry>;
    using Index = std::unordered_map<WordId, AssociatedDocuments>;

    namespace io {

        void Save(const Dictionary& dictionary, const std::string& file_name);
        Dictionary LoadDictionary(const std::string& file_name);

        void Save(const Index& index, const std::string& file_name);
        Index LoadIndex(const std::string& file_name);

    }  // namespace io

}  // namespace yindex
