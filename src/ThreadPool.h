#ifndef VOXELENGINE_THREADPOOL_H
#define VOXELENGINE_THREADPOOL_H

#include <atomic>
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>
#include <utility>

class ThreadPool {
public:
    ThreadPool() = delete;
    static void Start() {
        auto num_threads = std::thread::hardware_concurrency();
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(&ThreadPool::ThreadLoop);
        }
    }

    static void QueueJob(const std::function<void()>& job) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.emplace(job);
        lock.unlock();

        mutex_condition.notify_one();
    }

    static void Stop() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
        lock.unlock();

        mutex_condition.notify_all();
        for (std::thread& active_thread : threads) {
            active_thread.join();
        }
        threads.clear();
    }

    static bool busy() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        return !jobs.empty();
    }

    static void waitForAllJobs() {
        while (busy() || workingThreads != 0);
    }


private:
    static void ThreadLoop() {
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mutex);
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

    inline static bool should_terminate = false;           // Tells threads to stop looking for jobs
    inline static std::mutex queue_mutex;                  // Prevents data races to the job queue
    inline static std::mutex counter_mutex;                // Prevents data races to the working threads counter
    inline static std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    inline static std::vector<std::thread> threads;
    inline static std::queue<std::function<void()>> jobs;
    inline static int workingThreads = 0;                  // Keep track of how many threads are currently working
};

#endif
