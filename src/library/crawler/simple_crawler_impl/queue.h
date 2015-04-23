#pragma once

#include "urls.h"

#include <library/save_load/save_load.h>

#include <algorithm>
#include <iterator>
#include <mutex>
#include <type_traits>
#include <vector>


namespace ycrawler {

    namespace sci {

        class UrlsQueue {
        public:
            void Push(const url::UrlIdWithTries& data) {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                heap_.push_back(data);
                std::push_heap(heap_.begin(), heap_.end());
            }

            template <typename It>
            void PushMany(It begin ,It end) {
                static_assert(
                    std::is_same<url::UrlId, typename std::iterator_traits<It>::value_type>::value,
                    "Wrong type"
                );
                std::lock_guard<std::mutex> lock_guard{mutex_};
                for (; begin != end; ++begin) {
                    heap_.push_back({*begin, 0});
                    std::push_heap(heap_.begin(), heap_.end());
                }
            }

            template <typename T>
            void PushMany(const T& data) {
                static_assert(
                    std::is_same<url::UrlId, typename T::value_type>::value, "Wrong type"
                );
                std::lock_guard<std::mutex> lock_guard{mutex_};
                for (const auto& value: data) {
                    heap_.push_back({value, 0});
                    std::push_heap(heap_.begin(), heap_.end());
                }
            }

            struct Response {
                bool empty = true;
                url::UrlIdWithTries url_with_tries = {};

                Response() = default;
                Response(const bool empty_other, const url::UrlIdWithTries ulr_id_with_tries_other)
                    : empty{empty_other}
                    , url_with_tries{ulr_id_with_tries_other} {
                }
            };

            Response Pop() {
                std::lock_guard<std::mutex> lock_guard{mutex_};

                if (heap_.empty()) {
                    return {};
                }

                std::pop_heap(heap_.begin(), heap_.end());
                const auto value = heap_.back();
                heap_.pop_back();
                return { false, value };
            }

            bool Empty() const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                return heap_.empty();
            }

            auto Size() const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                return heap_.size();
            }

            void Load(const std::string& file_name);

            void Save(const std::string& file_name) const;

        private:
            mutable std::mutex mutex_;
            std::vector<url::UrlIdWithTries> heap_;
        };

    }  // namespace sci

}  // namespace ycrawler


namespace ysave_load {

    template <>
    void Save<>(
        const std::vector<ycrawler::sci::url::UrlIdWithTries>& data, const std::string& file_name
    );


    namespace detail {
        template <>
        struct LoadImpl<std::vector<ycrawler::sci::url::UrlIdWithTries>> {
            static std::vector<ycrawler::sci::url::UrlIdWithTries> Do(const std::string& file_name);
        };
    }  // namespace detail

}  // namesapce ysave_load
