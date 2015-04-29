#pragma once

#include "detail.h"

#include <mutex>
#include <utility>
#include <vector>


namespace ycrawler {

    namespace sci {

        template <typename T>
        class VectorWithMutex : public WithMutex {
        public:
            VectorWithMutex() = default;
            void Push(const T& value) {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                data_.push_back(value);
            }

            void Push(T&& value) {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                data_.push_back(std::forward<T>(value));
            }

            auto Size() const {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                return data_.size();
            }

            std::vector<T>&& Get() {
                return std::move(data_);
            }

            void Set(std::vector<T>&& data) {
                data_ = std::forward<std::vector<T>>(data);
            }

        private:
            std::vector<T> data_;
        };

    }  // namespace sci

}  // namespace ycrawler

