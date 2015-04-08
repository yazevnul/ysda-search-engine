#include <library/download/interface.h>
#include <library/download/wget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <streambuf>
#include <string>
#include <unordered_set>

#include <cstdlib>


std::unordered_set<std::string> GetSimpleWikipediaUrls(const std::string& page) {
    // yes, I've read this thread http://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags
    std::regex simple_wikipedia_url_re(
        R"swiki(<a\s+href="(/wiki/[^"]+)"\s+[^<>]*>[^<>]*</a>)swiki",
        std::regex::ECMAScript | std::regex::icase | std::regex::optimize
    );

    std::unordered_set<std::string> urls;

    auto urls_begin = std::sregex_iterator(page.cbegin(), page.cend(), simple_wikipedia_url_re);
    const auto urls_end = std::sregex_iterator();
    for (auto it = urls_begin; urls_end != it; ++it) {
        auto match = *it;
        urls.insert(match[1]);  // actual URL is at index 1
    }

    return urls;
}


std::string ReadFile(const std::string& file_name) {
    std::ifstream file(file_name);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}



int main(const int argc, const char* argv[]) {
    if (2 != argc) {
        std::cerr << "Usage: links_getter url" << std::endl;
        return EXIT_FAILURE;
    }

    const auto url_to_download = std::string{argv[1]};
    const auto page = std::string{"downloaded_page.html"};

    const auto downloader = std::make_unique<ydownload::WgetDownloader>();
    const auto response = downloader->Download(url_to_download, page);

    if (response.Success) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "FAIL" << std::endl;
        return EXIT_FAILURE;
    }

    const auto file_content = ReadFile(page);
    const auto urls_in_file = GetSimpleWikipediaUrls(file_content);

    std::cout << "URLs found: " << urls_in_file.size() << '\n';
    for (auto&& url: urls_in_file) {
        std::cout << "http://simple.wikipedia.org" << url << '\n';
    }

    return EXIT_SUCCESS;
}
