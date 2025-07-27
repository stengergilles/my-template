#include "../platform/worker.hpp"
#include "logger.h"

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

void Worker::postTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(task));
        m_condition.notify_one();
    }
}

void Worker::threadLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(lock, [this] { return !m_tasks.empty() || !m_running; });

            if (!m_running && m_tasks.empty()) {
                break;
            }
            task = m_tasks.front();
            m_tasks.pop();
        }
        try {
            task();
        } catch (const std::exception& e) {
            LOG_ERROR("Worker thread task failed: %s", e.what());
        } catch (...) {
            LOG_ERROR("Worker thread task failed with unknown exception");
        }
    }
}
