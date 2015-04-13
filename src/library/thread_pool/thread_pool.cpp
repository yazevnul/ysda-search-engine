#include "thread_pool.h"

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <cstdint>


ythread_pool::ThreadPool::ThreadPool(const std::uint32_t threads) {
    {
        // Should use lock to ensure memory fence before workers have started
        std::lock_guard<std::mutex> lock_guard{mutex_};
        should_quit_ = false;
    }
    for (auto index = std::uint32_t{0}; index < threads; ++index) {
        workers_.emplace_back([this](){ this->Run(); });
    }
}


ythread_pool::ThreadPool::~ThreadPool()
{
    Shutdown();
}


void ythread_pool::ThreadPool::Enqueue(Worker&& worker)
{
    {
        std::lock_guard<std::mutex> lock_guard{mutex_};
        work_queue_.push(std::move(worker));
    }
    has_more_work_.notify_one();
}


bool ythread_pool::ThreadPool::Dequeue(Worker& worker)
{
    std::unique_lock<std::mutex> lock_guard{mutex_};

    while (!should_quit_ && work_queue_.empty()) {
        has_more_work_.wait(lock_guard);
    }

    if (should_quit_) {
        return false;
    } else {
        worker = std::move(work_queue_.front());
        work_queue_.pop();
        return true;
    }
}


void ythread_pool::ThreadPool::Run()
{
    while (true) {
        try {
            auto&& worker = Worker{};
            if (!Dequeue(worker)) {
                return;
            }

            worker();
        } catch (std::exception& exc) {
            std::cerr << "ThreadPool: Caught exception in worker function: "
                      << exc.what() << std::endl;
        } catch (...) {
            std::cerr << "ThreadPool: Caught unknown exception in worker function" << std::endl;
        }
    }
}


void ythread_pool::ThreadPool::Shutdown()
{
    {
        std::lock_guard<std::mutex> lock_fuard{mutex_};
        should_quit_ = true;
    }
    has_more_work_.notify_all();
    for (auto& thread: workers_) {
        thread.join();
    }

    workers_.clear();
}

