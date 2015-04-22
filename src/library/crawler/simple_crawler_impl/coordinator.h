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
                std::unique_lock<std::mutex> lock{add_job_mutex_};
                for(; !stop_condition && jobs_running_count_ > 0;) {
                    for (; jobs_limit_ && jobs_running_count_;) {
                        ++jobs_running_count_;
                        thread_pool_->enqueue([this] { this->Do(); });
                    }
                    add_job_.wait(lock);
                }
            }

            void Do() {
                worker_();
                --jobs_running_count_;
                add_job_.notify_one();
            }

            Worker worker_;

            std::unique_ptr<ThreadPool> thread_pool_;
            std::uint32_t jobs_limit_;
            std::atomic<std::uint32_t> jobs_running_count_;
            std::condition_variable add_job_;
            std::mutex add_job_mutex_;
        };

    }  // namespace sci

}  // namespace ycrawler
