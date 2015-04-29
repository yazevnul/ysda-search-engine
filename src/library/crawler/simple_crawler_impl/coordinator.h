#pragma once

#include "detail.h"

#include <third_party/ThreadPool/ThreadPool.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>

#include <cstdint>


namespace ycrawler {

    namespace sci {

        class Coordnator : public WithLock {
        public:
            using Worker = std::function<void()>;
            using Condition = std::function<bool()>;

            Coordnator() = default;
            virtual ~Coordnator() noexcept = default;

            void Run(Worker worker, const std::uint32_t jobs_limit, Condition stop_condition) {
                thread_pool_ = std::make_unique<ThreadPool>(jobs_limit);
                jobs_limit_ = jobs_limit;
                worker_ = worker;
                jobs_running_count_ = {};
                Coordinate(stop_condition);
            }

        private:
            void Coordinate(Condition stop_condition) {
                for(;;) {
                    {
                        std::lock_guard<std::mutex> lock_guard{jobs_running_count_mutex_};
                        for (; jobs_running_count_ < jobs_limit_; ++jobs_running_count_) {
                            thread_pool_->enqueue([this] { this->Do(); });
                        }
                    }
                    add_job_cv_.wait(object_lock_);
                    {
                        std::lock_guard<std::mutex> lock_guard{jobs_running_count_mutex_};
                        if (stop_condition && 0 == jobs_running_count_) {
                            break;
                        }
                    }
                }
            }

            void Do() {
                worker_();
                {
                    std::lock_guard<std::mutex> lock_guard{jobs_running_count_mutex_};
                    --jobs_running_count_;
                    add_job_cv_.notify_one();
                }
            }

            std::unique_ptr<ThreadPool> thread_pool_;
            std::uint32_t jobs_limit_ = {};

            Worker worker_;

            std::mutex jobs_running_count_mutex_;
            std::uint32_t jobs_running_count_ = {};

            std::condition_variable add_job_cv_;
        };

    }  // namespace sci

}  // namespace ycrawler
