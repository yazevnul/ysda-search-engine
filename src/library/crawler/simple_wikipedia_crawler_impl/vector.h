#pragma once

#include <mutex>
#include <utility>
#include <vector>

namespace yswci {

    template <typename T>
    class VectorWithMutex {
    public:
        void Push(const T& value) {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            data_.push_back(value);
        }

        void Push(T&& value) {
            std::lock_guard<std::mutex> lock_guard{mutex_};

            data_.push_back(std::forward<T>(value));
        }

        std::vector<T> Get() {
            return std::move<std::vector<T>>(data_);
        }

    private:
        std::vector<T> data_;
        std::mutex mutex_;
    };

}  // namespace yswci

