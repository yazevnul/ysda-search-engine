#include "urls.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include <cassert>


void ysci::UrlsQueue::Load() {
    std::lock_guard<std::mutex> lock_guard{mutex_};

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


void ysci::UrlsQueue::Save() const {
    std::lock_guard<std::mutex> lock_guard{mutex_};

    auto&& output = std::ofstream{file_name_};
    for (const auto& value: heap_) {
        output << value.id << '\t' << value.tries;
    }
}


void ysci::UrlToId::Load() {
    std::lock_guard<std::mutex> lock_guard{mutex_};

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


void ysci::UrlToId::Save() const {
    std::lock_guard<std::mutex> lock_guard{mutex_};

    auto&& output = std::ofstream{file_name_};
    for (const auto& value: direct_) {
        output << value.first << '\t' << value.second;
    }
}

