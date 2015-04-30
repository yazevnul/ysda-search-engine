#pragma once

#include "detail.h"

#include <third_party/ThreadPool/ThreadPool.h>

#include <condition_variable>
#include <functional>
#include <memory>

#include <cstdint>


namespace ycrawler {

    namespace sci {

        class Coordinator : public WithLock {
        public:
            using Worker = std::function<void()>;
            using ConditionChecker = std::function<bool()>;
            using BackupTask = std::function<void()>;

            Coordinator() = default;
            virtual ~Coordinator() noexcept = default;

            void Run(
                Worker worker, const std::uint32_t jobs_limit,
                ConditionChecker stop_condition_checker, BackupTask backup_task,
                const std::uint32_t backup_freq
            );

        private:
            void Coordinate();
            void RunWorker();

            std::unique_ptr<ThreadPool> thread_pool_;
            std::uint32_t jobs_limit_ = {};
            std::uint32_t backup_freq_ = {};

            Worker worker_;
            ConditionChecker stop_condition_checker_;
            BackupTask backup_task_;

            std::mutex jobs_running_count_mutex_;
            std::uint32_t jobs_running_count_ = {};
            std::uint32_t jobs_until_backup_ = {};

            std::condition_variable add_job_cv_;
        };

    }  // namespace sci

}  // namespace ycrawler
