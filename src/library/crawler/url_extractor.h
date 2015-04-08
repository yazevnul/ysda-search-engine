#pragma once

#include "url_extractor_interface.h"

#include <string>
#include <unordered_set>

namespace ycrawler {

    std::unordered_set<std::string> GetSimpleWikipediaUrls(const std::string& page);


    class SimpleWikipediaUrlExtractor: public IUrlExtractor {
    public:
        virtual std::unordered_set<std::string> Extract(const std::string& page) override {
            return GetSimpleWikipediaUrls(page);
        }

        virtual ~SimpleWikipediaUrlExtractor() {
        }
    };

}  // namespace ycrawler

