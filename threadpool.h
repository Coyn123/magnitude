#pragma once
#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>

class ThreadPool {
    public:
        explicit ThreadPool(size_t thread_count);
        ~ThreadPool();

        void submit(std::function<void()> task);
        void wait_idle();

    private:
        void worker_loop();

        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queue_mutex;

        std::condition_variable task_available;
        std::condition_variable pool_idle;

        size_t active_count = 0;
        bool stop = false;

};
