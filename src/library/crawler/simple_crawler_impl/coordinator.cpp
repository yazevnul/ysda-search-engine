#include "coordinator.h"

#include <third_party/ThreadPool/ThreadPool.h>

#include <future>
#include <memory>
#include <mutex>

#include <cstdint>


void ycrawler::sci::Coordinator::Run(
    Worker worker, const std::uint32_t jobs_limit, ConditionChecker stop_condition_checker,
    BackupTask backup_task, const std::uint32_t backup_freq
){
    thread_pool_ = std::make_unique<ThreadPool>(jobs_limit);
    jobs_limit_ = jobs_limit;
    backup_freq_ = backup_freq;
    worker_ = worker;
    stop_condition_checker_ = stop_condition_checker;
    backup_task_ = backup_task;
    jobs_running_count_ = {};
    jobs_until_backup_ = backup_freq_;
    Coordinate();
}


void ycrawler::sci::Coordinator::Coordinate() {
    for(;;) {
        {
            std::lock_guard<std::mutex> lock_guard{jobs_running_count_mutex_};
            if (0 == jobs_running_count_ && 0 == jobs_until_backup_) {
                // for the sake of the state consistency we can perform backup task only
                // when there are no jobs running
                std::async(backup_task_).wait();
                jobs_until_backup_ = backup_freq_;
            }
            const auto should_stop = stop_condition_checker_();
            const auto can_enqueue_jobs = !should_stop && jobs_until_backup_ > 0;
            if (can_enqueue_jobs) {
                for (; jobs_running_count_ < jobs_limit_; ++jobs_running_count_) {
                    thread_pool_->enqueue([this]{ this->RunWorker(); });
                }
            }
        }
        add_job_cv_.wait(object_lock_);
        {
            std::lock_guard<std::mutex> lock_guard{jobs_running_count_mutex_};
            const auto should_stop = stop_condition_checker_();
            if (should_stop && 0 == jobs_running_count_) {
                break;
            }
        }
    }
}


void ycrawler::sci::Coordinator::RunWorker() {
    worker_();
    {
        std::lock_guard<std::mutex> lock_guard{jobs_running_count_mutex_};
        --jobs_running_count_;
        if (jobs_until_backup_ > 0) {
            --jobs_until_backup_;
        }
        add_job_cv_.notify_one();
    }
}

