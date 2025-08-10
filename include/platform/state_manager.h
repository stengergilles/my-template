#pragma once

#include <string>
#include <map>
#include "nlohmann/json.hpp"

#include <mutex>
#include <condition_variable>
#include "worker.hpp" // For Worker::getInstance().postTask

class StateManager {
public:
    static StateManager& getInstance();

    // Save/Load window position
    void saveWindowPosition(const std::string& windowName, float x, float y);
    bool loadWindowPosition(const std::string& windowName, float& x, float& y);

    // Generic save/load for string data
    void saveString(const std::string& key, const std::string& value);
    bool loadString(const std::string& key, std::string& value);

    // Load all state from file asynchronously
    void loadStateAsync();
    // Save all state to file asynchronously
    void saveStateAsync();
    // Save all state to file synchronously
    void saveState();

    // Set the internal data path for file storage (Android specific)
    void setInternalDataPath(const std::string& path);
    const std::string& getInternalDataPath() const { return m_internalDataPath; }
    bool isStateLoaded() const { return m_stateLoaded.load(); }
    void resetStateLoaded() { m_stateLoaded.store(false); }

    private:
    StateManager();
    ~StateManager();
    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;

    std::string m_internalDataPath;
    std::string m_stateFilePath;
    std::map<std::string, std::string> m_state;
    std::mutex m_mutex;
    std::atomic<bool> m_stateLoaded;

    void updateStateFilePath();
    void loadStateInternal(); // Internal synchronous load
    void saveStateInternal(); // Internal synchronous save
};
