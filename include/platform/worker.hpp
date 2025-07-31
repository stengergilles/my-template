
#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future> // Required for std::future and std::packaged_task

class Worker {
public:
    static Worker& getInstance();

    // Delete copy constructor and assignment operator
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    ~Worker();

    std::future<void> postTask(std::function<void()> task);

private:
    Worker();
    void threadLoop();

    std::queue<std::packaged_task<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_workerThread;
    bool m_running;
};
