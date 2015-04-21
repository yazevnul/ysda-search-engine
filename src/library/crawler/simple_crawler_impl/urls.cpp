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
        parser >> value;
        data.push_back(value);
    }
    return data;
}


template <>
void ysave_load::Save<>(
    const std::vector<ycrawler::sci::url::UrlIdWithTries>& data, const std::string& file_name
){
    auto&& output = std::ofstream{file_name};
    for (const auto& value: data) {
        output << value.id << '\t' << value.tries << '\n';
    }
}


std::vector<ycrawler::sci::url::UrlIdWithTries>
ysave_load::detail::LoadImpl<std::vector<ycrawler::sci::url::UrlIdWithTries>>::Do(
    const std::string& file_name
){
    auto data = std::vector<ycrawler::sci::url::UrlIdWithTries>{};
    auto&& input = std::ifstream{file_name};
    for (auto line = std::string{}; std::getline(input, line);) {
        auto value = ycrawler::sci::url::UrlIdWithTries{};
        auto&& parser = std::stringstream{line};

        parser >> value.id >> value.tries;
        data.push_back(value);
    }
    return data;
}


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


// ysave_load::{Load,Save} specializations BEGIN


void ycrawler::sci::UrlsQueue::Load(const std::string& file_name) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    heap_ = ysave_load::Load<std::vector<url::UrlIdWithTries>>(file_name);
    std::make_heap(heap_.begin(), heap_.end());
}


void ycrawler::sci::UrlsQueue::Save(const std::string& file_name) const {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    ysave_load::Save(heap_, file_name);
}


void ycrawler::sci::UrlToId::Load(const std::string& file_name) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    direct_ = ysave_load::Load<std::unordered_map<std::string, url::UrlId>>(file_name);
    reverse_.clear();
    for (const auto& value: direct_) {
        reverse_.insert({value.second, value.first});
    }
}


void ycrawler::sci::UrlToId::Save(const std::string& file_name) const {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    ysave_load::Save(direct_, file_name);
}

