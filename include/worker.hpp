
#pragma once

#include <future>
#include <functional>
#include <thread>
#include <utility> // For std::forward

template<typename R>
class Worker {
public:
    Worker() : m_running(false) {}

    template<typename Func, typename Callback>
    void submit(Func&& func, Callback&& callback) {
        if (m_running) {
            // Optionally, handle this case (e.g., queue the task, log a warning)
            return;
        }
        m_running = true;
        m_future = std::async(std::launch::async, [this, f = std::forward<Func>(func), cb = std::forward<Callback>(callback)]() mutable {
            R result = f();
            // Execute callback on the main thread or handle synchronization if needed
            // For simplicity, we'll execute it directly here. In a real GUI app,
            // you might post this to the main thread's event queue.
            cb(result);
            m_running = false;
        });
    }

    bool is_running() const {
        return m_running;
    }

private:
    std::future<void> m_future; // Changed to void as result is handled by callback
    bool m_running;
};
