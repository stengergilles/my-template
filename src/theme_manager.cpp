#include "../include/theme_manager.h"
#include "../include/logger.h"
#include "imgui.h"
#include "imgui_internal.h" // For ImGui::GetStyle()
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // For ICON_MIN_FA, ICON_MAX_FA

ThemeManager::ThemeManager()
{
    setupDefaultThemes();
    // Do not apply a default theme here, it will be applied after ImGui context is created
}

ThemeManager::~ThemeManager()
{
    // No specific cleanup needed for ImGui fonts as they are managed by ImGui context
}

void ThemeManager::setupDefaultThemes()
{
    // Dark Theme
    Theme darkTheme;
    darkTheme.name = "Dark";
    darkTheme.screen_background = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey
    darkTheme.widget_background = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Slightly lighter grey
    m_availableThemes.push_back(darkTheme);

    // Light Theme
    Theme lightTheme;
    lightTheme.name = "Light";
    lightTheme.screen_background = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Light grey
    lightTheme.widget_background = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Slightly darker grey
    m_availableThemes.push_back(lightTheme);

    // Custom Theme (example)
    Theme customTheme;
    customTheme.name = "Custom";
    customTheme.screen_background = ImVec4(0.15f, 0.05f, 0.2f, 1.0f); // Purple-ish dark
    customTheme.widget_background = ImVec4(0.3f, 0.1f, 0.4f, 1.0f); // Purple-ish light
    m_availableThemes.push_back(customTheme);
}

void ThemeManager::applyTheme(const Theme& theme)
{
    m_currentTheme = theme;
    applyImGuiStyle(theme);
    LOG_INFO("Applied theme: %s", theme.name.c_str());
}

void ThemeManager::applyImGuiStyle(const Theme& theme)
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Colors
    style.Colors[ImGuiCol_WindowBg] = theme.screen_background;
    style.Colors[ImGuiCol_FrameBg] = theme.widget_background;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(theme.widget_background.x + 0.1f, theme.widget_background.y + 0.1f, theme.widget_background.z + 0.1f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(theme.widget_background.x + 0.2f, theme.widget_background.y + 0.2f, theme.widget_background.z + 0.2f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f); // Green checkmark
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.8f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White text

    // Styling
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.ChildRounding = 3.0f;

    // Spacing
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
}

void ThemeManager::showThemeEditor()
{
    if (ImGui::Begin("Theme Editor")) {
        // Theme selection dropdown
        if (ImGui::BeginCombo("Select Theme", m_currentTheme.name.c_str())) {
            for (const auto& theme : m_availableThemes) {
                bool is_selected = (m_currentTheme.name == theme.name);
                if (ImGui::Selectable(theme.name.c_str(), is_selected)) {
                    applyTheme(theme);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Color pickers for current theme (if it's 'Custom' or editable)
        if (m_currentTheme.name == "Custom") {
            ImGui::ColorEdit3("Screen Background", (float*)&m_currentTheme.screen_background);
            ImGui::ColorEdit3("Widget Background", (float*)&m_currentTheme.widget_background);
            if (ImGui::Button("Apply Custom Theme")) {
                applyTheme(m_currentTheme);
            }
        }

        ImGui::End();
    }
}

void ThemeManager::loadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Clear existing fonts

    // Load default font (DroidSans.ttf) - 12px
    // This is already handled in platformInit() for Android, but we'll add it here for completeness
    // and to ensure it's the first font in the atlas.
    // The path should be relative to the assets folder for Android.
    // For other platforms, it might be a direct file path.
#ifdef __ANDROID__
    // For Android, fonts are loaded from assets. The platform layer handles this.
    // We'll assume the default font is loaded by the platform.
    // If we need to load custom fonts from assets, we'd need a mechanism to pass AssetManager here.
    // For now, we'll rely on the platform's font loading for the default.
    ImFont* defaultFont = io.Fonts->AddFontDefault();
    if (defaultFont) {
        m_fonts["Default"] = defaultFont;
        LOG_INFO("Loaded default ImGui font.");
    } else {
        LOG_ERROR("Failed to load default ImGui font.");
    }
#else
    // For non-Android platforms, load from file system
    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF("../external/fonts/DroidSans.ttf", 12.0f);
    if (defaultFont) {
        m_fonts["Default"] = defaultFont;
        LOG_INFO("Loaded default font: DroidSans.ttf");
    } else {
        LOG_ERROR("Failed to load default font: ../external/fonts/DroidSans.ttf");
    }
#endif

    // Load FontAwesome font (fa-solid-900.ttf) - 12px
    // This is typically loaded with a merge to the default font for icons.
    // Ensure the path is correct for your setup.
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

#ifdef __ANDROID__
    // For Android, FontAwesome is also loaded from assets. The platform layer handles this.
    // We'll assume it's merged with the default font by the platform.
    // If not, you'd need to adjust your platform's font loading.
    // For now, we'll rely on the platform's font loading for FontAwesome.
    // If you need to explicitly load it here, you'd need access to the Android AssetManager.
    // As a fallback, we'll try to add it directly if the platform doesn't.
    ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 12.0f, &config, icon_ranges);
    if (fontAwesome) {
        m_fonts["FontAwesome"] = fontAwesome;
        LOG_INFO("Loaded FontAwesome font.");
    } else {
        LOG_ERROR("Failed to load FontAwesome font: fa-solid-900.ttf");
    }
#else
    ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF("../external/fontawesome/fa-solid-900.ttf", 12.0f, &config, icon_ranges);
    if (fontAwesome) {
        m_fonts["FontAwesome"] = fontAwesome;
        LOG_INFO("Loaded FontAwesome font: fa-solid-900.ttf");
    } else {
        LOG_ERROR("Failed to load FontAwesome font: ../external/fontawesome/fa-solid-900.ttf");
    }
#endif

    io.Fonts->Build();
}
