#include <library/download/interface.h>
#include <library/download/wget.h>

#include <iostream>
#include <memory>

int main() {
    const auto downloader = std::make_unique<ydownload::WgetDownloader>();
    const auto response = downloader->Download(
        "http://simple.wikipedia.org/wiki/Main_Page",
        "docs/main_page.html"
    );

    if (response.Success)
        std::cout << "OK" << std::endl;
    else
        std::cout << "FAIL" << std::endl;

    return 0;
}
