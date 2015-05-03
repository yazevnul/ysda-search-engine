#include <third_party/ThreadPool/ThreadPool.h>
#include <third_party/cxxopts/src/cxxopts.hpp>

#include <algorithm>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdint>
#include <cstdlib>
#include <cctype>


// trim from start
static inline std::string& TrimLeft(std::string& str) {
    const static auto criterion = [](const char value){ return !std::isspace(value); };
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::cref(criterion)));
    return str;
}


// trim from end
static inline std::string& TrimRight(std::string& str) {
    static const auto criterion = [](const char value){ return !std::isspace(value); };
    str.erase(std::find_if(str.rbegin(), str.rend(), std::cref(criterion)).base(), str.end());
    return str;
}


// trim from both ends
static inline std::string& Trim(std::string& str) {
    return TrimLeft(TrimRight(str));
}


std::vector<std::string> RetrieveFileNames(const std::string& file_name) {
    auto&& input = std::ifstream{file_name};
    auto file_names = std::vector<std::string>{};
    for (auto line = std::string{}; std::getline(input, line);) {
        file_names.push_back(Trim(line));
    }
    return file_names;
}


static std::unordered_map<std::string, std::uint32_t> CalculateFrequencies(std::istream& input) {
    static constexpr char SPACE = ' ';
    auto word_to_freq = std::unordered_map<std::string, std::uint32_t>{};
    for (auto word = std::string{}; std::getline(input, word, SPACE);) {
        Trim(word);
        if (!word.empty()) {
            ++word_to_freq[word];
        }
    }
    return word_to_freq;
}


static inline std::unordered_map<std::string, std::uint32_t> CalculateFrequencies(
    const std::string& file_name
){
    auto&& input = std::ifstream{file_name};
    return CalculateFrequencies(input);
}


static inline void Sum(
    std::unordered_map<std::string, std::uint32_t>& to,
    const std::unordered_map<std::string, std::uint32_t>& from
){
    for (const auto& value: from) {
        to[value.first] += value.second;
    }
}


static inline void ProcessFile(
    const std::string& file_name,
    std::unordered_map<std::string, std::uint32_t>& word_to_freq, std::mutex& word_to_freq_mutex
){
    const auto new_word_to_freq = CalculateFrequencies(file_name);

    std::lock_guard<std::mutex> lock_guard{word_to_freq_mutex};
    Sum(word_to_freq, new_word_to_freq);
}


static std::unordered_map<std::string, std::uint32_t> ProcessInput(
    const std::vector<std::string>& input_files, const std::uint32_t jobs_count
){
    auto word_to_freq = std::unordered_map<std::string, std::uint32_t>{};
    std::mutex word_to_freq_mutex;
    ThreadPool thread_pool{jobs_count};
    for (const auto& input_file: input_files) {
        thread_pool.enqueue([&input_file, &word_to_freq, &word_to_freq_mutex]{
            ProcessFile(input_file, word_to_freq, word_to_freq_mutex);
        });
    }
    return word_to_freq;
}


static inline void PrintWordFrequencies(
    const std::unordered_map<std::string, std::uint32_t>& word_to_freq,
    std::ofstream& output
){
    for (const auto value: word_to_freq) {
        output << value.first << '\t' << value.second << '\n';
    }
}


static inline void PrintWordFrequencies(
    const std::unordered_map<std::string, std::uint32_t>& word_to_freq,
    const std::string& output_file_name
){
    auto&& output = std::ofstream{output_file_name};
    PrintWordFrequencies(word_to_freq, output);
}


struct Args {
    std::string input_file_name;
    std::string output_file_name;
    std::uint32_t jobs_count = 1;
};


Args ParseOptions(int argc, char* argv[]) {
    auto options = cxxopts::Options{argv[0], std::string{"\n  "} + "Count word occurences"};
    auto args = Args{};
    options.add_options()(
        "i,input",
        "File with list of files",
        cxxopts::value<>(args.input_file_name),
        "FILE"
    )(
        "o,output",
        "Where to save word word frequencies. format: word \\t frequiecy",
        cxxopts::value<>(args.output_file_name),
        "FILE"
    )(
        "j,jobs",
        "Jobs count",
        cxxopts::value<>(args.jobs_count),
        "INT"
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
    const auto file_names = RetrieveFileNames(args.input_file_name);
    const auto word_to_freq = ProcessInput(file_names, args.jobs_count);
    PrintWordFrequencies(word_to_freq, args.output_file_name);
    return EXIT_SUCCESS;
}
