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
    SettingsManager();
    ~SettingsManager();

    void applySettings(const Settings& settings);
    void showSettingsEditor();
    #ifdef __ANDROID__
    void loadFonts(AAssetManager* assetManager = nullptr);
#else
    void loadFonts();
#endif
    bool loadSettingsFromState(); // New public method

    // Getters for settings properties
    ImVec4 getScreenBackground() const { return m_currentSettings.screen_background; }
    const std::vector<Settings>& getAvailableSettings() const { return m_availableSettings; }
    const std::vector<std::string>& getAvailableFontNames() const { return m_availableFontNames; }
    const std::vector<float>& getAvailableFontSizes() const { return m_availableFontSizes; }
    float getScale() const { return m_currentSettings.scale; }

private:
    Settings m_currentSettings;
    std::vector<Settings> m_availableSettings;
    std::map<std::string, ImFont*> m_fonts;
    std::vector<std::string> m_availableFontNames;
    std::vector<float> m_availableFontSizes;
    #ifndef __ANDROID__
    std::map<std::string, std::shared_ptr<LinuxAsset>> m_fontData;
#else
    std::map<std::string, std::vector<char>> m_fontData; // New member to store font data
#endif

    void setupDefaultSettings();
    void applyImGuiStyle(const Settings& settings);
};
