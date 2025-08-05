#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include <map>
#ifdef __ANDROID__
#include <android/asset_manager.h> // Required for AAssetManager and AAsset
#else
#include <memory>
#include "../include/platform/linux/asset_manager.h"
#endif

// Structure to hold settings
struct Settings
{
    std::string name;
    ImVec4 screen_background;
    ImVec4 widget_background;
    float corner_roundness; // New member for corner roundness
    std::string font_name; // New member for selected font name
    float font_size;       // New member for selected font size
    float scale;           // New member for UI scale adjustment
    // Add more settings properties as needed
};

class SettingsManager
{
public:
    static SettingsManager& getInstance();

    // Delete copy constructor and assignment operator
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    ~SettingsManager();

private:
    SettingsManager();

public:
    void initialize();
    void applySettings(const Settings& settings);
    void reapplyCurrentStyle();
    void showSettingsEditor();
    void updateAvailableFonts();
    void loadSettings();
    void saveSettingsAsync();
    

private:
    bool loadSettingsFromState(Settings& loadedSettings);
    void applyLoadedSettings(const Settings& settings);
    void saveSettingsInternal(const Settings& settings);

    // Getters for settings properties
public:
    ImVec4 getScreenBackground() const { return m_currentSettings.screen_background; }
    const std::string& getFontName() const { return m_currentSettings.font_name; }
    float getFontSize() const { return m_currentSettings.font_size; }
    const std::vector<std::string>& getAvailableFontNames() const { return m_availableFontNames; }
    const std::vector<float>& getAvailableFontSizes() const { return m_availableFontSizes; }
    float getScale() const { return m_currentSettings.scale; }

private:
    Settings m_currentSettings;
    std::vector<Settings> m_availableSettings;
    std::vector<std::string> m_availableFontNames;
    std::vector<float> m_availableFontSizes;

    void setupDefaultSettings();
    void applyImGuiStyle(const Settings& settings);
};
