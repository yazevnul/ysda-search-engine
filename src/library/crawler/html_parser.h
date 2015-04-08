#pragma once

#include <string>
#include <unordered_set>

namespace ycrawler {

    std::unordered_set<std::string> GetSimpleWikipediaUrls(const std::string& page);

}  // namespace NCrawler

