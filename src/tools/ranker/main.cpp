#include <library/accurate_accumulate/kahan_accumulator.h>
#include <library/index/dictionary.pb.h>
#include <library/index/index.h>
#include <library/index/index.pb.h>
#include <library/protobuf_helpers/serialization.h>
#include <library/timer/timer.h>

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>


//! Will skip empty tokens
std::vector<std::string> Split(const std::string& str, const char delimiter) {
    std::vector<std::string> result;

    std::istringstream parser(str);
    for (std::string value; std::getline(parser, value, delimiter);)
        if (! value.empty())
            result.push_back(value);

    return result;
}


inline std::string ToLower(const std::string& str) {
    std::string result(str);
    std::transform(str.cbegin(), str.cend(), result.begin(), tolower);
    return result;
}


std::vector<std::string> ToLower(const std::vector<std::string>& words) {
    std::vector<std::string> result(words.size());
    for (size_t index = 0; index < words.size(); ++index)
        result[index] = ToLower(words[index]);
    return result;
}


struct WordIdWithFrequency {
    yindex::WordId word_id = 0;
    uint64_t frequency = 0;
};


inline std::ostream& operator <<(std::ostream& output, const WordIdWithFrequency& value) {
    output << '(' << value.word_id << ", " << value.frequency << ')';
    return output;
}


using BagOfWords = std::vector<WordIdWithFrequency>;


//! Will ignore unknown words
BagOfWords ToBagOfWords(
    const std::vector<std::string>& words,
    const yindex::Dictionary& dictionary
){
    std::unordered_map<yindex::WordId, uint64_t> word_to_frequency;
    for (const auto& word: words) {
        const auto word_it = dictionary.find(word);
        if (dictionary.end() == word_it)
            continue;

        word_to_frequency[word_it->second.word_id()] += 1;
    }

    BagOfWords word_with_frequency;
    for (const auto& value: word_to_frequency) {
        WordIdWithFrequency dummy;
        dummy.word_id = value.first;
        dummy.frequency = value.second;
        word_with_frequency.push_back(dummy);
    }

    return word_with_frequency;
}


//! Will ignore unknown words
std::vector<yindex::DocumentId> BooleanSearch(
    const BagOfWords& bow,
    const yindex::InvertedIndex& index,
    const bool exact_search
){
    std::unordered_set<yindex::DocumentId> relevant_documents;

    // We need to find the first word in index to initialize `relevant_documents`
    auto bow_it = bow.begin();
    for (; bow.cend() != bow_it; ++bow_it) {
        const auto it = index.find(bow_it->word_id);
        if (index.cend() == it)
            continue;

        for (int jindex = 0; jindex < it->second.documents_size(); ++jindex)
            relevant_documents.insert(it->second.documents(jindex).document_id());

        break;
    }

    std::unordered_set<yindex::DocumentId> relevant_documents_next;
    for (; bow.end() != bow_it; ++bow_it) {
        const auto it = index.find(bow_it->word_id);
        if (index.cend() == it)
            continue;

        relevant_documents_next.clear();
        for (int jindex = 0; jindex < it->second.documents_size(); ++jindex) {
            const bool unknown_document = relevant_documents.end()
                == relevant_documents.find(it->second.documents(jindex).document_id());
            if (unknown_document)
                continue;

            relevant_documents_next.insert(it->second.documents(jindex).document_id());
        }

        //! This word gave us an empty set of documents, so we ignore it if `exact_search=false`
        if (!exact_search && relevant_documents_next.empty())
            continue;

        relevant_documents.swap(relevant_documents_next);
    }

    return {relevant_documents.cbegin(), relevant_documents.cend()};
}


template <typename TIt>
void Print(const TIt begin, const TIt end, std::ostream& output) {
    output << "[ ";
    for (auto it = begin; end != it; ++it) {
        if (begin != it)
            output << ", ";

        output << *it;
    }

    output << " ]";
}


template <typename T>
inline void Print(const T& container, std::ostream& output) {
    Print(container.begin(), container.end(), output);
}


struct DocumentIdWithRelevance {
    yindex::DocumentId document_id = 0;
    double relevance = 0;
};


inline bool operator <(const DocumentIdWithRelevance& lhs, const DocumentIdWithRelevance& rhs) {
    return std::tie(lhs.relevance, lhs.document_id) < std::tie(rhs.relevance, rhs.document_id);
}


inline std::ostream& operator <<(std::ostream& output, const DocumentIdWithRelevance& value) {
    output << '(' << value.document_id << ", " << value.relevance << ')';
    return output;
}


struct BM25Parameters {
    double k_one = 0;
    double k_three = 0;
    double b = 0;
};


double CalculateBM25(
    const BagOfWords& bow,
    const yindex::DocumentId document_id,
    const yindex::InvertedIndex& index,
    const uint64_t number_of_documents,
    const BM25Parameters& bm25_parameters
){
    double bm25 = 0;
    for (const auto& word: bow) {
        const auto& associated_documents = index.at(word.word_id);
        // log((N - n + 0.5) / (n + 0.5))
        double summand = log(
            (0.5 + number_of_documents - associated_documents.documents_size())
            / (0.5 + associated_documents.documents_size())
        );

        // (k_3 + 1) * q / (k_3 + q)
        summand *= (bm25_parameters.k_three + 1) * word.frequency
                   / (bm25_parameters.k_three + word.frequency + 0.5);

        double within_document_frequency = 0;
        double L = 0;
        for (int jindex = 0; jindex < associated_documents.documents_size(); ++jindex) {
            if (associated_documents.documents(jindex).document_id() == document_id) {
                within_document_frequency = associated_documents.documents(jindex).word_frequency();
                L = associated_documents.documents(jindex).dl_by_avg_dl();
            }
        }

        const double K = bm25_parameters.k_one
                         * (bm25_parameters.b * L + (1.0 - bm25_parameters.b));
        // (k_1 + 1) * f / (K + f)
        summand *= (bm25_parameters.k_one + 1) * within_document_frequency
                   / (K + within_document_frequency + 0.5);

        bm25 += summand;
    }

    return bm25;
}


std::vector<DocumentIdWithRelevance> CalculateBM25(
    const BagOfWords& bow,
    const std::vector<yindex::DocumentId>& documents,
    const yindex::InvertedIndex& index,
    const uint64_t number_of_documents,
    const BM25Parameters& bm25_parameters
){
    // This may be done in a much more efficient manner, but we leave this that simple for now

    std::vector<DocumentIdWithRelevance> result;
    for (const auto document_id: documents) {
        DocumentIdWithRelevance dummy;
        dummy.document_id = document_id;
        dummy.relevance = CalculateBM25(
            bow, document_id, index, number_of_documents, bm25_parameters
        );
        result.push_back(dummy);
    }
    std::sort(result.rbegin(), result.rend());

    return result;
}


int main(int argc, char** argv) {
    const std::vector<std::string> args{argv + 1, argv + argc};

    ytimer::Timer timer;

    std::cout << "Loading data..." << std::endl;
    timer.Start();
    const auto dictionary = yindex::io::LoadDictionary(args[0]);
    const auto index = yindex::io::LoadIndex(args[1]);
    const auto index_statistics = yproto::ReadDelimitedFromFile<yindex::IndexStatistics>(args[2]);
    std::cout << "Loading data...DONE in " << timer.Stop().count() << std::endl;

    std::cout << "Dictionary size is " << dictionary.size()
              << ", index size is " << index.size()
              << ", number of documents is " << index_statistics.number_of_documents()
              << ", average document length is " << index_statistics.average_document_length()
              << std::endl;

    for (std::string query; std::getline(std::cin, query);) {
        const auto words = ToLower(Split(query, ' '));
        std::cout << "Splitted query: ";
        Print(words, std::cout);
        std::cout << std::endl;

        const auto bow = ToBagOfWords(words, dictionary);
        std::cout << "Bag of words: ";
        Print(bow, std::cout);
        std::cout << std::endl;

        const auto fuzzy_boolean_documents = BooleanSearch(bow,index, false);
        std::cout << "Documents (fuzzy boolean search) [" << fuzzy_boolean_documents.size() << "]:";
        Print(fuzzy_boolean_documents, std::cout);
        std::cout << std::endl;

        const auto exact_boolean_documents = BooleanSearch(bow, index, true);
        std::cout << "Documents (exact boolean search) [" << exact_boolean_documents.size() << "]:";
        Print(exact_boolean_documents, std::cout);
        std::cout << std::endl;

        BM25Parameters bm25_parameters;
        bm25_parameters.k_one = 1.2;
        bm25_parameters.k_three = 100;
        bm25_parameters.b = 0.75;

        const auto bm25_fuzzy_boolean_documents = CalculateBM25(
            bow, fuzzy_boolean_documents, index, index_statistics.number_of_documents(),
            bm25_parameters
        );
        std::cout << "Documents BM25(fuzzy boolean search) [" << bm25_fuzzy_boolean_documents.size()
                  << "]:";
        Print(bm25_fuzzy_boolean_documents, std::cout);
        std::cout << std::endl;

        const auto bm25_exact_boolean_documents = CalculateBM25(
            bow, exact_boolean_documents, index, index_statistics.number_of_documents(),
            bm25_parameters
        );
        std::cout << "Documents BM25(exact boolean search) [" << bm25_exact_boolean_documents.size()
                  << "]:";
        Print(bm25_exact_boolean_documents, std::cout);
        std::cout << std::endl;
    }

    return 0;
}
