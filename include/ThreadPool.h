#ifndef VOXELENGINE_THREADPOOL_H
#define VOXELENGINE_THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool() = delete;
    static void Start();

    static void QueueJob(const std::function<void()>& job);

    static void Stop();

    static bool busy();

    static void waitForAllJobs();

private:
    static void ThreadLoop();

    inline static bool should_terminate = false;           // Tells threads to stop looking for jobs
    inline static std::mutex queue_mutex;                  // Prevents data races to the job queue
    inline static std::mutex counter_mutex;                // Prevents data races to the working threads counter
    inline static std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    inline static std::vector<std::thread> threads;
    inline static std::queue<std::function<void()>> jobs;
    inline static int workingThreads = 0;                  // Keep track of how many threads are currently working
};

#endif
