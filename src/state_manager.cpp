#include "../include/state_manager.h"
#include "../include/logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>



StateManager::StateManager() : m_internalDataPath(".") {
    updateStateFilePath();
    loadState();
}

StateManager::~StateManager() {
    saveState();
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
    m_stateFilePath = m_internalDataPath + "/app_state.ini";
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

void StateManager::loadState() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(m_stateFilePath);
    if (file.is_open()) {
        m_state.clear();
        std::string line;
        while (std::getline(file, line)) {
            size_t eqPos = line.find("=");
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                m_state[key] = value;
            }
        }
        file.close();
        
    } else {
        
    }
}

void StateManager::saveState() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ofstream file(m_stateFilePath);
    if (file.is_open()) {
        for (const auto& pair : m_state) {
            file << pair.first << "=" << pair.second << "\n";
        }
        file.close();
        
    } else {
        LOG_ERROR("Failed to save state to %s", m_stateFilePath.c_str());
    }
}
