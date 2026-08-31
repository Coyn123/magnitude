#include "threadpool.h"
#include <condition_variable>
#include <mutex>
#include <thread>

ThreadPool::ThreadPool (size_t thread_count) {
    size_t i = 0;
    while (i < thread_count) {
        workers.emplace_back(&ThreadPool::worker_loop, this);
        i++;
    }

}

ThreadPool::~ThreadPool() {

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
        task_available.notify_all();
    }
    for (auto& worker : workers) {
        worker.join();
    }

}

void ThreadPool::worker_loop() {
    for(;;) {
        std::function<void()> new_task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            task_available.wait(lock, [this] { return this->stop == true || !this->tasks.empty();  });
            if (stop and tasks.empty()) return;
            new_task = tasks.front();
            tasks.pop();
            active_count++;
        }
        new_task();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            active_count--;
            if(active_count == 0 and tasks.empty()) {
                pool_idle.notify_one();
            }
        }
    }
}

void ThreadPool::submit(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(task);
    }
    task_available.notify_one();
};

void ThreadPool::wait_idle() {
    {
    std::unique_lock<std::mutex> lock(queue_mutex);
    pool_idle.wait(lock, [this] { return this->active_count == 0 and this->tasks.empty();  });
    }
    return;
}
