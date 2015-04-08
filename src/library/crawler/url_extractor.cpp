#include "url_extractor.h"

#include <regex>
#include <string>
#include <unordered_set>


// yes, I've read this thread http://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags
static const std::regex SIMPLE_WIKIPEDIA_URL_REGEX{
    R"swiki(<a\s+href="(/wiki/[^"]+)"\s+[^<>]*>[^<>]*</a>)swiki",
    std::regex::ECMAScript | std::regex::icase | std::regex::optimize
};


std::unordered_set<std::string> ycrawler::GetSimpleWikipediaUrls(const std::string& page) {
    std::unordered_set<std::string> urls;

    static const auto prefix = std::string{"http://simple.wikipedia.org"};

    auto urls_begin = std::sregex_iterator(page.cbegin(), page.cend(), SIMPLE_WIKIPEDIA_URL_REGEX);
    const auto urls_end = std::sregex_iterator();
    for (auto it = urls_begin; urls_end != it; ++it) {
        urls.insert(prefix + (*it)[1].str());  // actual URL is at index 1
    }

    return urls;
}
