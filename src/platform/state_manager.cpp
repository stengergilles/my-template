#include "../../include/platform/state_manager.h"
#include "../../include/platform/logger.h"
#include "../../include/platform/worker.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

StateManager::StateManager() : m_internalDataPath("."), m_stateLoaded(false) {
    LOG_INFO("StateManager constructor called.");
    updateStateFilePath();
    // State loading will be triggered externally after path is set
}


StateManager::~StateManager() {
    LOG_INFO("StateManager destructor called.");
    // Save state synchronously on shutdown to avoid race conditions with the worker thread
    // during static deinitialization.
    saveStateInternal();
}

StateManager& StateManager::getInstance() {
    static StateManager instance;
    return instance;
}

void StateManager::setInternalDataPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_internalDataPath = path;
    updateStateFilePath();
}

void StateManager::updateStateFilePath() {
    m_stateFilePath = m_internalDataPath + "/app_state.json";
}

void StateManager::saveWindowPosition(const std::string& windowName, float x, float y) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << x << "," << y;
    m_state["window_pos_" + windowName] = ss.str();
}

bool StateManager::loadWindowPosition(const std::string& windowName, float& x, float& y) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string key = "window_pos_" + windowName;
    if (m_state.count(key)) {
        std::string value = m_state[key];
        size_t commaPos = value.find(",");
        if (commaPos != std::string::npos) {
            try {
                x = std::stof(value.substr(0, commaPos));
                y = std::stof(value.substr(commaPos + 1));
                return true;
            } catch (const std::exception& e) {
                LOG_ERROR("Error parsing window position for %s: %s", windowName.c_str(), e.what());
            }
        }
    }
    return false;
}

void StateManager::saveString(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_state[key] = value;
}

bool StateManager::loadString(const std::string& key, std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_state.count(key)) {
        value = m_state[key];
        return true;
    }
    return false;
}

void StateManager::loadStateInternal() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(m_stateFilePath);
    if (file.is_open()) {
        try {
            nlohmann::json j;
            file >> j;
            m_state.clear();
            for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
                m_state[it.key()] = it.value().get<std::string>();
            }
        } catch (const nlohmann::json::exception& e) {
            LOG_ERROR("Error parsing state file %s: %s", m_stateFilePath.c_str(), e.what());
        }
        file.close();
    } else {
        LOG_INFO("State file %s not found, creating new one on save.", m_stateFilePath.c_str());
    }
}

void StateManager::saveStateInternal() {
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG_INFO("Attempting to save state to %s", m_stateFilePath.c_str());
    std::ofstream file(m_stateFilePath);
    if (file.is_open()) {
        nlohmann::json j = m_state;
        file << std::setw(4) << j << std::endl;
        file.close();
        LOG_INFO("State successfully saved to %s", m_stateFilePath.c_str());
    } else {
        LOG_ERROR("Failed to save state to %s", m_stateFilePath.c_str());
    }
}

void StateManager::loadStateAsync() {
    LOG_INFO("StateManager::loadStateAsync() called.");
    Worker::getInstance().postTask([this]() {
        loadStateInternal();
        m_stateLoaded.store(true);
    });
}



void StateManager::saveStateAsync() {
    LOG_INFO("StateManager::saveStateAsync() called.");
    Worker::getInstance().postTask([this]() {
        saveStateInternal();
    });
}

void StateManager::saveState() {
    LOG_INFO("StateManager::saveState() called.");
    saveStateInternal();
}
