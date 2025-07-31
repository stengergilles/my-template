#include "../include/platform/worker.hpp"
#include <future> // Required for std::packaged_task

Worker& Worker::getInstance() {
    static Worker instance;
    return instance;
}

Worker::Worker() : m_running(true) {
    m_workerThread = std::thread(&Worker::threadLoop, this);
}

Worker::~Worker() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_running = false;
        m_condition.notify_all();
    }
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

std::future<void> Worker::postTask(std::function<void()> task) {
    std::packaged_task<void()> packaged_task(std::move(task)); // Move the std::function into packaged_task
    std::future<void> future = packaged_task.get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(packaged_task));
        m_condition.notify_one();
    }
    return future;
}

void Worker::threadLoop() {
    while (true) {
        std::packaged_task<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(lock, [this] { return !m_tasks.empty() || !m_running; });

            if (!m_running && m_tasks.empty()) {
                return;
            }
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}
