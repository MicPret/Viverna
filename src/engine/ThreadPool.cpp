#include <viverna/core/ThreadPool.hpp>

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace verna {

void ThreadPool::ThreadFunction() {
    std::function<void()> task;
    while (true) {
        {
            std::unique_lock lock(tasks_mtx);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
                return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

ThreadPool& ThreadPool::Get() {
    static ThreadPool singleton(std::thread::hardware_concurrency());
    return singleton;
}

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    threads.reserve(num_threads);
    for (size_t i = 0; i < num_threads; i++)
        threads.emplace_back(&ThreadPool::ThreadFunction, this);
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& t : threads)
        t.join();
}

}  // namespace verna
