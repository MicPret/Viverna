#ifndef VERNA_THREAD_POOL_HPP
#define VERNA_THREAD_POOL_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace verna {

class ThreadPool {
   public:
    template <class F, class... Args>
    auto Enqueue(F&& f, Args&&... args)
        -> std::future<decltype(f(std::forward<Args>(args)...))>;
    ~ThreadPool();
    static ThreadPool& Get();

   private:
    ThreadPool(size_t num_threads);
    void ThreadFunction();
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable condition;
    bool stop;
};

template <class F, class... Args>
auto ThreadPool::Enqueue(F&& f, Args&&... args) {
    using ret_t = decltype(f(std::forward<Args>(args)...));
    auto task = std::make_shared<std::packaged_task<ret_t()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future result = task->get_future();
    {
        std::lock_guard lock(tasks_mtx);
        if (!stop)
            tasks.emplace([task]() { (*task)(); });
        else
            return std::future<ret_t>();
    }
    condition.notify_one();
    return result;
}
}  // namespace verna

#endif
