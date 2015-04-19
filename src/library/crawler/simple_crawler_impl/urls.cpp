#include "urls.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <cassert>


void ycrawler::ysci::UrlsQueue::Load() {
    std::lock_guard<std::mutex> lock_guard{mutex_};

    if (file_name_.empty()) {
        throw std::runtime_error{"file name is empty"};
    }

    heap_.clear();

    auto&& input = std::ifstream{file_name_};
    for (auto line = std::string{}; std::getline(input, line);) {
        auto value = url::UrlIdWithTries{};
        auto&& parser = std::stringstream{line};

        parser >> value.id >> value.tries;
        heap_.push_back(value);
    }

    std::make_heap(heap_.begin(), heap_.end());
}


void ycrawler::ysci::UrlsQueue::Save() const {
    std::lock_guard<std::mutex> lock_guard{mutex_};

    if (file_name_.empty()) {
        throw std::runtime_error{"file name is empty"};
    }

    auto&& output = std::ofstream{file_name_};
    for (const auto& value: heap_) {
        output << value.id << '\t' << value.tries << '\n';
    }
}


void ycrawler::ysci::UrlToId::Load() {
    std::lock_guard<std::mutex> lock_guard{mutex_};

    if (file_name_.empty()) {
        throw std::runtime_error{"file name is empty"};
    }

    direct_.clear();
    reverse_.clear();

    auto&& input = std::ifstream{file_name_};
    for (auto line = std::string{}; std::getline(input, line);) {
        auto&& parser = std::stringstream{line};

        auto url = std::string{};
        auto id = url::UrlId{};
        parser >> url >> id;

        assert(direct_.find(url) == direct_.cend());
        assert(reverse_.find(id) == reverse_.cend());

        direct_.insert({url, id});
        reverse_.insert({id, url});
    }
}


void ycrawler::ysci::UrlToId::Save() const {
    std::lock_guard<std::mutex> lock_guard{mutex_};

    if (file_name_.empty()) {
        throw std::runtime_error{"file name is empty"};
    }

    auto&& output = std::ofstream{file_name_};
    for (const auto& value: direct_) {
        output << value.first << '\t' << value.second << '\n';
    }
}


void ycrawler::ysci::url::Save(
    const std::vector<UrlId>& data, const std::string& file_name
){
    auto&& output = std::ofstream{file_name};
    for (const auto value: data) {
        output << value << '\n';
    }
}


std::vector<ycrawler::ysci::url::UrlId> ycrawler::ysci::url::Load(
    const std::string& file_name
){
    auto data = std::vector<ycrawler::ysci::url::UrlId>{};
    auto&& input = std::ifstream{file_name};
    for (auto line = std::string{}; std::getline(input, line);) {
        auto&& parser = std::stringstream{line};
        auto value = ycrawler::ysci::url::UrlId{};
        parser >> value;
        data.push_back(value);
    }
    return data;
}

