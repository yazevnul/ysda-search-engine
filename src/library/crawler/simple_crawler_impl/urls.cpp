#include "urls.h"

#include <library/save_load/save_load.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>


// ysave_load::{Load,Save} specializations BEGIN


template <>
void ysave_load::Save<>(
    const std::vector<ycrawler::sci::url::UrlId>& data, const std::string& file_name
){
    auto&& output = std::ofstream{file_name};
    for (const auto value: data) {
        output << value << '\n';
    }
}


std::vector<ycrawler::sci::url::UrlId>
ysave_load::detail::LoadImpl<std::vector<ycrawler::sci::url::UrlId>>::Do(
    const std::string& file_name
){
    auto data = std::vector<ycrawler::sci::url::UrlId>{};
    auto&& input = std::ifstream{file_name};
    for (auto line = std::string{}; std::getline(input, line);) {
        auto&& parser = std::stringstream{line};
        auto value = ycrawler::sci::url::UrlId{};
        if (parser >> value) {
            data.push_back(value);
        }
    }
    return std::move(data);
}


// ysave_load::{Load,Save} specializations BEGIN


