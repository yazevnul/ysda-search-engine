#include "queue.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>


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
    return std::move(data);
}


void ycrawler::sci::UrlsQueue::Load(const std::string& file_name) {
    std::lock_guard<std::mutex> lock_guard{object_mutex_};
    heap_ = ysave_load::Load<std::vector<url::UrlIdWithTries>>(file_name);
    std::make_heap(heap_.begin(), heap_.end());
}


void ycrawler::sci::UrlsQueue::Save(const std::string& file_name) const {
    std::lock_guard<std::mutex> lock_guard{object_mutex_};
    ysave_load::Save(heap_, file_name);
}

