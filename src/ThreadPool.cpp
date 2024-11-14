#include "../include/ThreadPool.h"

void ThreadPool::Start() {
    const auto num_threads = std::thread::hardware_concurrency();
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(&ThreadPool::ThreadLoop);
    }
}

void ThreadPool::QueueJob(const std::function<void()> &job) {
    std::unique_lock lock(queue_mutex);
    jobs.emplace(job);
    lock.unlock();

    mutex_condition.notify_one();
}

void ThreadPool::Stop() {
    std::unique_lock lock(queue_mutex);
    should_terminate = true;
    lock.unlock();

    mutex_condition.notify_all();
    for (std::thread& active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
}

bool ThreadPool::busy() {
    std::unique_lock lock(queue_mutex);
    return !jobs.empty();
}

void ThreadPool::waitForAllJobs() {
    while (busy() || workingThreads != 0);
}

void ThreadPool::ThreadLoop() {
    while (true) {
        std::unique_lock lock(queue_mutex);
        mutex_condition.wait(lock, [] {
            return !jobs.empty() || should_terminate;
        });

        if (should_terminate)
            return;

        auto job = jobs.front();
        jobs.pop();
        lock.unlock();

        counter_mutex.lock();
        workingThreads++;
        counter_mutex.unlock();

        job();

        counter_mutex.lock();
        workingThreads--;
        counter_mutex.unlock();
    }
}

