#include "url_to_id.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>


template <>
void ysave_load::Save<>(
    const std::unordered_map<std::string, ycrawler::sci::url::UrlId>& data,
    const std::string& file_name
){
    auto&& output = std::ofstream{file_name};
    for (const auto& value: data) {
        output << value.first << '\t' << value.second << '\n';
    }
}


std::unordered_map<std::string, ycrawler::sci::url::UrlId>
ysave_load::detail::LoadImpl<std::unordered_map<std::string, ycrawler::sci::url::UrlId>>::Do(
    const std::string& file_name
){
    auto data = std::unordered_map<std::string, ycrawler::sci::url::UrlId>{};
    auto&& input = std::ifstream{file_name};
    for (auto line = std::string{}; std::getline(input, line);) {
        auto&& parser = std::stringstream{line};
        auto url = std::string{};
        auto id = ycrawler::sci::url::UrlId{};
        parser >> url >> id;

        data.insert({std::move(url), id});
    }
    return data;
}


void ycrawler::sci::UrlToId::Load(const std::string& file_name) {
    std::lock_guard<std::mutex> lock_guard{object_mutex_};
    direct_ = ysave_load::Load<std::unordered_map<std::string, url::UrlId>>(file_name);
    reverse_.clear();
    for (const auto& value: direct_) {
        reverse_.insert({value.second, value.first});
    }
}


void ycrawler::sci::UrlToId::Save(const std::string& file_name) const {
    std::lock_guard<std::mutex> lock_guard{object_mutex_};
    ysave_load::Save(direct_, file_name);
}

