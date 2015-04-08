#include <library/crawler/url_extractor.h>
#include <library/download/interface.h>
#include <library/download/wget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>

#include <cstdlib>


std::string ReadFile(const std::string& file_name) {
    std::ifstream file(file_name);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}



int main(const int argc, const char* argv[]) {
    if (2 != argc) {
        std::cerr << "Usage: links_parser url" << std::endl;
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
    const auto urls_in_file = ycrawler::GetSimpleWikipediaUrls(file_content);

    std::cout << "URLs found: " << urls_in_file.size() << '\n';
    for (auto&& url: urls_in_file) {
        std::cout << url << '\n';
    }

    return EXIT_SUCCESS;
}
