#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <vector>

#include <cstdint>


namespace ythread_pool {

    class ThreadPool {
    public:
        using Worker = std::function<void()>;

        explicit ThreadPool(const std::uint32_t threads);
        ~ThreadPool();
        void Enqueue(Worker&& worker);

    private:
        void Shutdown();
        void Run();
        bool Dequeue(Worker& worker);

        std::vector<std::thread> workers_;
        std::queue<Worker> work_queue_;
        bool should_quit_;

        std::mutex mutex_;
        std::condition_variable has_more_work_;
    };

} // namespace ythread_pool

