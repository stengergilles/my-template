#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include <map>
#include <android/asset_manager.h> // Required for AAssetManager and AAsset

// Structure to hold theme settings
struct Theme
{
    std::string name;
    ImVec4 screen_background;
    ImVec4 widget_background;
    float corner_roundness; // New member for corner roundness
    std::string font_name; // New member for selected font name
    float font_size;       // New member for selected font size
    // Add more theme properties as needed
};

class ThemeManager
{
public:
    ThemeManager();
    ~ThemeManager();

    void applyTheme(const Theme& theme);
    void showThemeEditor();
    void loadFonts(AAssetManager* assetManager = nullptr);

    // Getters for theme properties
    ImVec4 getScreenBackground() const { return m_currentTheme.screen_background; }
    const std::vector<Theme>& getAvailableThemes() const { return m_availableThemes; }
    const std::vector<std::string>& getAvailableFontNames() const { return m_availableFontNames; }
    const std::vector<float>& getAvailableFontSizes() const { return m_availableFontSizes; }

private:
    Theme m_currentTheme;
    std::vector<Theme> m_availableThemes;
    std::map<std::string, ImFont*> m_fonts;
    std::vector<std::string> m_availableFontNames;
    std::vector<float> m_availableFontSizes;
    std::map<std::string, std::vector<char>> m_fontData; // New member to store font data

    void setupDefaultThemes();
    void applyImGuiStyle(const Theme& theme);
};
