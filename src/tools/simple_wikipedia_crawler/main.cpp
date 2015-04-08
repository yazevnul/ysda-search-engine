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


// One day there would be JSON, but TSV for now
ycrawler::SimpleWikipediaCrawler::Config ParseConfig(const std::string& file_name) {
    auto config = ycrawler::SimpleWikipediaCrawler::Config{};

    // parsing
    std::ifstream input{file_name.c_str()};
    for (std::string line; std::getline(input, line);) {
        if (line.empty()) {
            continue;
        }

        auto parser = std::stringstream{line};
        auto field_name = std::string{};
        if (!std::getline(parser, field_name, '\t')) {
            throw std::runtime_error("Failed to retrieve field name while parsing config");
        }

        if ("threads" == field_name) {
            parser >> config.threads;
        } else if ("state_directory" == field_name) {
            parser >> config.state_directory;
        } else if ("documents_directory" == field_name) {
            parser >> config.documents_directory;
        } else if ("documents_data_directory" == field_name) {
            parser >> config.documents_data_directory;
        } else if ("urls_seed" == field_name) {
            for (std::string url; std::getline(parser, url, '\t');) {
                config.urls_seed.push_back(url);
            }
        } else {
            throw std::runtime_error(std::string("Failed to parse config on field: ") + field_name);
        }
    }

    // validation
    if (0 == config.threads) {
        throw std::runtime_error("Number of threads is zero");
    }
    if (config.state_directory.empty()) {
        throw std::runtime_error("state_directory is empty");
    }
    if (config.documents_directory.empty()) {
        throw std::runtime_error("documents_directory is empty");
    }
    if (config.documents_data_directory.empty()) {
        throw std::runtime_error("documents_data_directory is empty");
    }
    if (config.urls_seed.empty()) {
        throw std::runtime_error("URLs seed is empty!");
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
