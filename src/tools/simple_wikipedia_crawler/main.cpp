#include <third_party/json11/json11.hpp>

#include <library/crawler/simple_wikipedia_crawler.h>
#include <library/download/wget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>
#include <cstdlib>


std::string ReadFile(const std::string& file_name) {
    std::ifstream file(file_name);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}


ycrawler::SimpleWikipediaCrawler::Config ParseConfig(const std::string& file_name) {
    const auto json = [&file_name](){
        auto error = std::string{};
        const auto json = json11::Json::parse(ReadFile(file_name), error);
        if (json.is_null()) {
            throw std::runtime_error{error};
        }
        return json;
    }();
    if (!json.is_object()) {
        std::runtime_error{"Malformed config"};
    }

    auto config = ycrawler::SimpleWikipediaCrawler::Config{};

    for (const auto& kv: json.object_items()) {
        if ("threads" == kv.first) {
            config.threads = static_cast<std::uint32_t>(kv.second.int_value());
        } else if ("state_directory" == kv.first) {
            config.state_directory = kv.second.string_value();
        } else if ("documents_directory" == kv.first) {
            config.documents_directory = kv.second.string_value();
        } else if ("documents_data_directory" == kv.first) {
            config.documents_data_directory = kv.second.string_value();
        } else if ("urls_seed" == kv.second.string_value()) {
            for (const auto& value: kv.second.array_items()) {
                if (!value.is_string()) {
                    throw std::runtime_error{"Malformed config"};
                }
                config.urls_seed.push_back(value.string_value());
            }
        } else if ("tries_limit" == kv.first) {
            config.tries_limit = static_cast<std::uint32_t>(kv.second.int_value());
        }
    }

    // validation
    if (0 == config.threads) {
        throw std::runtime_error{"Number of threads is zero"};
    }
    if (config.state_directory.empty()) {
        throw std::runtime_error{"state_directory is empty"};
    }
    if (config.documents_directory.empty()) {
        throw std::runtime_error{"documents_directory is empty"};
    }
    if (config.documents_data_directory.empty()) {
        throw std::runtime_error{"documents_data_directory is empty"};
    }
    if (config.urls_seed.empty()) {
        throw std::runtime_error{"URLs seed is empty"};
    }
    if (0 == config.tries_limit) {
        throw std::runtime_error{"Number of tries is zero"};
    }
    return config;
}


std::unique_ptr<ycrawler::ICrawler> MakeCrawler(const std::vector<std::string>& args) {
    auto crawler = std::make_unique<ycrawler::SimpleWikipediaCrawler>();
    if (args[0] == "new") {
        if (args.size() != 2) {
            throw std::runtime_error("Usage: crawler config");
        }
        const auto config = ParseConfig(args[1]);
        crawler->SetConfig(config);
    } else if (args[0] == "restore") {
        if (args.size() != 2) {
            throw std::runtime_error("Usage: crawler state_directory");
        }
        crawler->Restore(args[1]);
    } else {
        throw std::runtime_error("Unknown mode");
    }

    crawler->SetDownloader(std::make_unique<ydownload::WgetDownloader>());
    return std::move(crawler);
}


int main(const int argc, const char* argv[]) {
    const auto args = std::vector<std::string>{argv + 1, argv + argc};
    auto crawler = MakeCrawler(args);

    crawler->Start();

    return EXIT_SUCCESS;
}
