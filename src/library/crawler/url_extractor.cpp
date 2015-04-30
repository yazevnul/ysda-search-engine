#include "url_extractor.h"

#include <algorithm>
#include <regex>
#include <string>
#include <unordered_set>


static bool EndsWith(const std::string& line, const std::string& suffix) {
    if (suffix.size() > line.size()) {
        return false;
    }
    return std::equal(
        line.cend() - static_cast<std::string::difference_type>(suffix.size()),
        line.cend(), suffix.cbegin(),
        [](const char lhs, const char rhs) {
            return std::tolower(lhs) == std::tolower(rhs);
    });
}


static bool IsHtmlPageUrl(const std::string& url) {
    static const std::vector<std::string> NOT_HTML_PAGES_ENDINGS = {
        ".jpeg",
        ".jpg",
        ".ogg",
        ".pdf",
        ".php",
        ".png",
        ".svg",
        ".webp"
    };

    for (const auto& suffix: NOT_HTML_PAGES_ENDINGS) {
        if (EndsWith(url, suffix)) {
            return false;
        }
    }
    return true;
}


//! some details may be found here http://en.wikipedia.org/wiki/Help:URL
static bool IsRelevantUrl(const std::string& url) {
    if (!IsHtmlPageUrl(url)) {
        return false;
    }

    // URLs with CGI parameters are probably irrelevant for us
    if (url.find('?') != std::string::npos) {
        return false;
    }

    const auto last_colon_position = url.find_last_of(':');
    if (last_colon_position != std::string::npos && last_colon_position > 5) {
        return false;
    }

    return true;
}

/*! http://simple.wikipedia.org/wiki/Music#Other_websites ->
 *  http://simple.wikipedia.org/wiki/Music
 */
static std::string NormalizeUrl(const std::string& url) {
    const auto sharp_pos = url.find_last_of('#');
    if (sharp_pos == std::string::npos) {
        return url;
    }
    return { url.cbegin(), url.cbegin() + static_cast<std::string::difference_type>(sharp_pos) };
}


std::unordered_set<std::string> ycrawler::GetSimpleWikipediaUrls(const std::string& page) {
    std::unordered_set<std::string> urls;

    static const auto prefix = std::string{"http://simple.wikipedia.org"};
    // yes, I've read this thread http://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags
    static const std::regex SIMPLE_WIKIPEDIA_URL_REGEX{
        R"swiki(<a\s+href="(/wiki/[^"]+)"\s+[^<>]*>[^<>]*</a>)swiki",
        std::regex::ECMAScript | std::regex::icase | std::regex::optimize
    };

    auto urls_begin = std::sregex_iterator(page.cbegin(), page.cend(), SIMPLE_WIKIPEDIA_URL_REGEX);
    const auto urls_end = std::sregex_iterator();
    for (auto it = urls_begin; urls_end != it; ++it) {
        const auto url = prefix + (*it)[1].str();  // actual URL is at index 1
        if (IsRelevantUrl(url)) {
            urls.insert(NormalizeUrl(url));
        }
    }

    return urls;
}
