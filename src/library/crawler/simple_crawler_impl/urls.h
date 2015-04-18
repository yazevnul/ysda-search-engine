#pragma once

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdint>

//! ysci stands for "simple crawler implementation"
namespace ysci {

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
                } else if (id < other.id) {
                    return true;
                } else if (id > other.id) {
                    return false;
                } else {
                    return false;
                }
            }
        };

    }  // namespace url


    class UrlsQueue {
    public:
        explicit UrlsQueue(const std::string& file_name)
            : file_name_{file_name} {
        }

        void Push(const url::UrlId id, const std::uint32_t tries = 0) {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            heap_.push_back({id, tries});
            std::push_heap(heap_.begin(), heap_.end());
        }

        url::UrlIdWithTries Pop() {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            std::pop_heap(heap_.begin(), heap_.end());
            const auto value = heap_.back();
            heap_.pop_back();
            return value;
        }

        void Load();

        void Save() const;

    private:
        std::string file_name_;
        std::vector<url::UrlIdWithTries> heap_;
        mutable std::mutex mutex_;
    };


    class UrlToId {
    public:
        explicit UrlToId(const std::string& file_name)
            : file_name_{file_name} {
        }

        bool Has(const std::string& url) const {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            return direct_.find(url) != direct_.end();
        }

        bool Has(const url::UrlId& id) const {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            return reverse_.find(id) != reverse_.cend();
        }

        void Add(const std::string& url) {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            const auto id = static_cast<url::UrlId>(direct_.size() + 1);
            direct_.insert({url, id});
            reverse_.insert({id, url});
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

        void Load();

        void Save() const;

    private:
        std::string file_name_;

        std::unordered_map<std::string, url::UrlId> direct_;
        std::unordered_map<url::UrlId, std::string> reverse_;

        mutable std::mutex mutex_;
    };

}  // namespace yswci

