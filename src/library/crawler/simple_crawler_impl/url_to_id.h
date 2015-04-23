#pragma once

#include "urls.h"

#include <library/save_load/save_load.h>

#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>


namespace ycrawler {

    namespace sci {

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

            struct Response {
                bool known = false;
                url::UrlId id = {};

                Response() = default;
                Response(const bool known_other, const url::UrlId id_other)
                    : known{known_other}
                    , id{id_other} {
                }
            };

            template <typename It>
            std::vector<Response> Add(It begin, It end) {
                static_assert(
                    std::is_same<
                        std::string,
                        typename std::remove_const<
                            typename std::iterator_traits<It>::value_type
                        >::type>::value,
                    "Iterator value type must be std::string"
                );

                std::lock_guard<std::mutex> lock_guard{mutex_};
                std::vector<Response> result;
                for (auto it = begin; it != end; ++it) {
                    const auto& url = *it;
                    auto response = Response{};
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
            mutable std::mutex mutex_;

            // this place worth optimization (store one copy of a string instead of two)
            std::unordered_map<std::string, url::UrlId> direct_;
            std::unordered_map<url::UrlId, std::string> reverse_;
        };

    }  // namespace sci

}  // namespace ycrawler


namespace ysave_load {

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
