
#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class Worker {
public:
    static Worker& getInstance();

    // Delete copy constructor and assignment operator
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    ~Worker();

    void postTask(std::function<void()> task);

private:
    Worker();
    void threadLoop();

    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_workerThread;
    bool m_running;
};
