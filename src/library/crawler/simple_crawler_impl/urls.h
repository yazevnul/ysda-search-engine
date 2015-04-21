#pragma once

#include <library/save_load/save_load.h>

#include <algorithm>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdint>


namespace ycrawler {

    //! ysci stands for "simple crawler implementation"
    namespace sci {

        namespace url {

            using UrlId = std::uint32_t;


            struct UrlIdWithTries {
                UrlId id;            // url identifier
                std::uint32_t tries; // number of failed download attempts

                UrlIdWithTries()
                    : id{0}
                    , tries{0} {
                }

                UrlIdWithTries(const UrlId id_other, const std::uint32_t tries_other)
                    : id{id_other}
                    , tries{tries_other} {
                }

                //! Lesser tries than better
                bool operator< (const UrlIdWithTries& other) const noexcept {
                    if (tries < other.tries) {
                        return false;
                    } else if (tries > other.tries) {
                        return true;
                    } else if (id > other.id) {
                        return true;
                    } else if (id < other.id) {
                        return false;
                    } else {
                        return false;
                    }
                }
            };

        }  // namespace url


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

            url::UrlIdWithTries Pop() {
                std::lock_guard<std::mutex> lock_guard{mutex_};

                std::pop_heap(heap_.begin(), heap_.end());
                const auto value = heap_.back();
                heap_.pop_back();
                return value;
            }

            void Load(const std::string& file_name);

            void Save(const std::string& file_name) const;

        private:
            std::vector<url::UrlIdWithTries> heap_;
            mutable std::mutex mutex_;
        };


        class UrlToId {
        public:
            bool Has(const std::string& url) const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                return direct_.find(url) != direct_.end();
            }

            bool Has(const url::UrlId& id) const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                return reverse_.find(id) != reverse_.cend();
            }

            struct AddResponse {
                bool known = false;
                url::UrlId id = {};
            };

            template <typename It>
            std::vector<AddResponse> Add(It begin, It end) {
                static_assert(
                    std::is_same<std::string, typename std::remove_const<typename std::iterator_traits<It>::value_type>::type>::value,
                    "Iterator value type must be std::string"
                );
                std::lock_guard<std::mutex> lock_guard{mutex_};
                std::vector<AddResponse> result;
                for (auto it = begin; it != end; ++it) {
                    const auto& url = *it;
                    auto response = AddResponse{};
                    auto jit = direct_.find(url);
                    response.known = (jit != direct_.end());
                    if (response.known) {
                        response.id = jit->second;
                    } else {
                        const auto id = static_cast<url::UrlId>(direct_.size() + 1);
                        direct_.insert({url, id});
                        reverse_.insert({id, url});
                        response.id = id;
                    }
                    result.push_back(response);
                }
                return std::move(result);
            }

            url::UrlId Get(const std::string& url) const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                auto it = direct_.find(url);
                if (direct_.end() == it) {
                    throw std::runtime_error{"Unable to get ID"};
                }
                return it->second;
            }

            std::string Get(const url::UrlId& id) const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                auto it = reverse_.find(id);
                if (reverse_.end() == it) {
                    throw std::runtime_error{"Unable to get URL"};
                }
                return it->second;
            }

            void Load(const std::string& file_name);

            void Save(const std::string& file_name) const;

        private:
            // this place worth optimization (store one copy of a string instead of two)
            std::unordered_map<std::string, url::UrlId> direct_;
            std::unordered_map<url::UrlId, std::string> reverse_;

            mutable std::mutex mutex_;
        };

    }  // namespace sci

}  // namespace ycrawler


namespace ysave_load {

    template <>
    void Save<>(const std::vector<ycrawler::sci::url::UrlId>& data, const std::string& file_name);


    namespace detail {
        template <>
        struct LoadImpl<std::vector<ycrawler::sci::url::UrlId>> {
            static std::vector<ycrawler::sci::url::UrlId> Do(const std::string& file_name);
        };
    }  // namespace detail


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


    template <>
    void Save<>(
        const std::unordered_map<std::string, ycrawler::sci::url::UrlId>& data,
        const std::string& file_name
    );


    namespace detail {
        template <>
        struct LoadImpl<std::unordered_map<std::string, ycrawler::sci::url::UrlId>> {
            static std::unordered_map<std::string, ycrawler::sci::url::UrlId> Do(
                const std::string& file_name
            );
        };
    }  // namespace detail

}  // namespace ysave_load

