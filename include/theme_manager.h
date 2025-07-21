#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include <map>

// Structure to hold theme settings
struct Theme
{
    std::string name;
    ImVec4 screen_background;
    ImVec4 widget_background;
    // Add more theme properties as needed
};

class ThemeManager
{
public:
    ThemeManager();
    ~ThemeManager();

    void applyTheme(const Theme& theme);
    void showThemeEditor();
    void loadFonts();

    // Getters for theme properties
    ImVec4 getScreenBackground() const { return m_currentTheme.screen_background; }
    const std::vector<Theme>& getAvailableThemes() const { return m_availableThemes; }

private:
    Theme m_currentTheme;
    std::vector<Theme> m_availableThemes;
    std::map<std::string, ImFont*> m_fonts;

    void setupDefaultThemes();
    void applyImGuiStyle(const Theme& theme);
};
