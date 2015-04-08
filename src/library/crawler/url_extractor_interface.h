#pragma once

#include <string>
#include <unordered_set>


namespace ycrawler {

    class IUrlExtractor {
    public:
        virtual std::unordered_set<std::string> Extract(const std::string& page) = 0;

        virtual ~IUrlExtractor() {
        }
    };

}  // namespace ycrawler

