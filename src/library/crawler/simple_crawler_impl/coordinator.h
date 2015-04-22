#pragma once

#include <third_party/ThreadPool/ThreadPool.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>

#include <cstdint>


namespace ycrawler {

    namespace sci {

        class Coordnator {
        public:
            using Worker = std::function<void()>;
            using Condition = std::function<bool()>;

            void Run(Worker worker, const std::uint32_t jobs_limit, Condition stop_condition) {
                jobs_limit_ = jobs_limit;
                worker_ = worker;
                thread_pool_ = std::make_unique<ThreadPool>(jobs_limit);
                Coordinate(stop_condition);
            }

        private:
            void Coordinate(Condition stop_condition) {
                std::unique_lock<std::mutex> add_job_cv_lock{add_job_cv_mutex_};
                for(;;) {
                    {
                        std::lock_guard<std::mutex> lock_guard{add_job_mutex_};
                        for (; jobs_running_count_ < jobs_limit_; ++jobs_running_count_) {
                            thread_pool_->enqueue([this] { this->Do(); });
                        }
                    }
                    add_job_cv_.wait(add_job_cv_lock);
                    {
                        std::lock_guard<std::mutex> lock_guard{add_job_mutex_};
                        if (stop_condition && 0 == jobs_running_count_) {
                            break;
                        }
                    }
                }
            }

            void Do() {
                worker_();
                {
                    std::lock_guard<std::mutex> lock_guard{add_job_mutex_};
                    --jobs_running_count_;
                    add_job_cv_.notify_one();
                }
            }

            Worker worker_;

            std::unique_ptr<ThreadPool> thread_pool_;
            std::uint32_t jobs_limit_ = {};
            std::uint32_t jobs_running_count_ = {};

            std::mutex add_job_mutex_;

            std::condition_variable add_job_cv_;
            std::mutex add_job_cv_mutex_;
        };

    }  // namespace sci

}  // namespace ycrawler
