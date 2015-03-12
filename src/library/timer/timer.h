#pragma once

#include <cassert>
#include <chrono>

namespace ytimer {

    class Timer {
    public:
        inline void Start() {
            assert(!running_);

            start_ = std::chrono::system_clock::now();
            running_ = true;
        }

        template <typename T = double>
        inline std::chrono::duration<T> Stop() {
            assert(running_);

            end_ = std::chrono::system_clock::now();
            running_ = false;
            return end_ - start_;
        }

        template <typename T = double>
        inline std::chrono::duration<T> Get() {
            assert(! running_);

            return end_ - start_;
        }

    private:
        bool running_ = false;
        std::chrono::time_point<std::chrono::system_clock> start_;
        std::chrono::time_point<std::chrono::system_clock> end_;
    };

}  // namespace ytimer

