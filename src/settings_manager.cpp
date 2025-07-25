#include "../include/settings_manager.h"
#include "../include/logger.h"
#include "../include/state_manager.h"
#include "../include/scaling_manager.h"
#include "imgui.h"
#include "imgui_internal.h" // For ImGui::GetStyle()
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // For ICON_MIN_FA, ICON_MAX_FA
#ifdef __ANDROID__
#include "platform/android/imgui_impl_android.h" // For ImGui_ImplAndroid_GetAvailableFontNames and Sizes
#endif

#ifndef __ANDROID__
#include "../include/platform/linux/asset_manager.h"
#endif

#include "../include/font_manager.h" // Include FontManager
#include "../include/worker.hpp"
#include "../include/platform/platform_base.h"

SettingsManager& SettingsManager::getInstance()
{
    static SettingsManager instance;
    return instance;
}

SettingsManager::SettingsManager()
{
    setupDefaultSettings();

#ifndef __ANDROID__
    // For non-Android platforms, provide a default list or load from elsewhere
    m_availableFontNames = {"DroidSans.ttf", "Cousine-Regular.ttf", "Karla-Regular.ttf"};
    m_availableFontSizes = {12.0f, 14.0f, 16.0f};
#endif
}

SettingsManager::~SettingsManager()
{
    // No specific cleanup needed for ImGui fonts as they are managed by ImGui context
}

void SettingsManager::setupDefaultSettings()
{
    // Dark Settings
    Settings darkSettings;
    darkSettings.name = "Dark";
    darkSettings.screen_background = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey
    darkSettings.widget_background = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Slightly lighter grey
    darkSettings.corner_roundness = 5.0f; // Default roundness for dark settings
    darkSettings.scale = 1.0f; // Default scale
    m_availableSettings.push_back(darkSettings);

    // Light Settings
    Settings lightSettings;
    lightSettings.name = "Light";
    lightSettings.screen_background = ImVec4(0.6f, 0.7f, 1.0f, 1.0f); // Light blue
    lightSettings.widget_background = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Slightly darker grey
    lightSettings.corner_roundness = 0.0f; // No roundness for light settings
    lightSettings.scale = 1.0f; // Default scale
    m_availableSettings.push_back(lightSettings);

    // Custom Settings (example)
    Settings customSettings;
    customSettings.name = "Custom";
    customSettings.screen_background = ImVec4(0.15f, 0.05f, 0.2f, 1.0f); // Purple-ish dark
    customSettings.widget_background = ImVec4(0.3f, 0.1f, 0.4f, 1.0f); // Purple-ish light
    customSettings.corner_roundness = 10.0f; // More roundness for custom settings
    customSettings.font_name = "DroidSans.ttf"; // Default font
    customSettings.font_size = 12.0f; // Default font size
    customSettings.scale = 1.0f; // Default scale
    m_availableSettings.push_back(customSettings);
}

void SettingsManager::applySettings(const Settings& settings)
{
    m_currentSettings = settings;
    applyImGuiStyle(settings);

    LOG_INFO("Applied settings: %s", settings.name.c_str());

    // Save settings to state manager
    StateManager::getInstance().saveString("settings_name", m_currentSettings.name);
    StateManager::getInstance().saveString("settings_screen_background_x", std::to_string(m_currentSettings.screen_background.x));
    StateManager::getInstance().saveString("settings_screen_background_y", std::to_string(m_currentSettings.screen_background.y));
    StateManager::getInstance().saveString("settings_screen_background_z", std::to_string(m_currentSettings.screen_background.z));
    StateManager::getInstance().saveString("settings_screen_background_w", std::to_string(m_currentSettings.screen_background.w));
    StateManager::getInstance().saveString("settings_widget_background_x", std::to_string(m_currentSettings.widget_background.x));
    StateManager::getInstance().saveString("settings_widget_background_y", std::to_string(m_currentSettings.widget_background.y));
    StateManager::getInstance().saveString("settings_widget_background_z", std::to_string(m_currentSettings.widget_background.z));
    StateManager::getInstance().saveString("settings_widget_background_w", std::to_string(m_currentSettings.widget_background.w));
    StateManager::getInstance().saveString("settings_corner_roundness", std::to_string(m_currentSettings.corner_roundness));
    StateManager::getInstance().saveString("settings_font_name", m_currentSettings.font_name);
    StateManager::getInstance().saveString("settings_font_size", std::to_string(m_currentSettings.font_size));
    StateManager::getInstance().saveString("settings_scale", std::to_string(m_currentSettings.scale));
    ScalingManager::getInstance().setScaleAdjustment(m_currentSettings.scale);
    FontManager::SetDefaultFont(m_currentSettings.font_name, m_currentSettings.font_size);
}

bool SettingsManager::loadSettingsFromState(Settings& loadedSettings)
{
    std::string settingsName;
    if (StateManager::getInstance().loadString("settings_name", settingsName)) {
        loadedSettings.name = settingsName;

        std::string val;
        if (StateManager::getInstance().loadString("settings_screen_background_x", val)) loadedSettings.screen_background.x = std::stof(val);
        if (StateManager::getInstance().loadString("settings_screen_background_y", val)) loadedSettings.screen_background.y = std::stof(val);
        if (StateManager::getInstance().loadString("settings_screen_background_z", val)) loadedSettings.screen_background.z = std::stof(val);
        if (StateManager::getInstance().loadString("settings_screen_background_w", val)) loadedSettings.screen_background.w = std::stof(val);

        if (StateManager::getInstance().loadString("settings_widget_background_x", val)) loadedSettings.widget_background.x = std::stof(val);
        if (StateManager::getInstance().loadString("settings_widget_background_y", val)) loadedSettings.widget_background.y = std::stof(val);
        if (StateManager::getInstance().loadString("settings_widget_background_z", val)) loadedSettings.widget_background.z = std::stof(val);
        if (StateManager::getInstance().loadString("settings_widget_background_w", val)) loadedSettings.widget_background.w = std::stof(val);

        if (StateManager::getInstance().loadString("settings_corner_roundness", val)) loadedSettings.corner_roundness = std::stof(val);
        if (StateManager::getInstance().loadString("settings_font_name", val)) loadedSettings.font_name = val;
        if (StateManager::getInstance().loadString("settings_font_size", val)) loadedSettings.font_size = std::stof(val);
        if (StateManager::getInstance().loadString("settings_scale", val)) loadedSettings.scale = std::stof(val);

        return true;
    }
    return false;
}

void SettingsManager::applyLoadedSettings(const Settings& settings)
{
    m_currentSettings = settings;
    applyImGuiStyle(settings);
    ScalingManager::getInstance().setScaleAdjustment(settings.scale);
    FontManager::SetDefaultFont(settings.font_name, settings.font_size);
    LOG_INFO("Applied loaded settings: %s", settings.name.c_str());
}


void SettingsManager::applyImGuiStyle(const Settings& settings)
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Colors
    style.Colors[ImGuiCol_WindowBg] = settings.screen_background;
    style.Colors[ImGuiCol_FrameBg] = settings.widget_background;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(settings.widget_background.x + 0.1f, settings.widget_background.y + 0.1f, settings.widget_background.z + 0.1f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(settings.widget_background.x + 0.2f, settings.widget_background.y + 0.2f, settings.widget_background.z + 0.2f, 1.0f);
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
    style.WindowRounding = settings.corner_roundness;
    style.FrameRounding = settings.corner_roundness;
    style.GrabRounding = settings.corner_roundness;
    style.PopupRounding = settings.corner_roundness;
    style.ScrollbarRounding = settings.corner_roundness;
    style.TabRounding = settings.corner_roundness;
    style.ChildRounding = settings.corner_roundness;

    // Spacing
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
}

void SettingsManager::showSettingsEditor()
{
    bool settings_changed = false;

    // Settings selection dropdown
    if (ImGui::BeginCombo("Select Settings", m_currentSettings.name.c_str())) {
        for (const auto& settings : m_availableSettings) {
            bool is_selected = (m_currentSettings.name == settings.name);
            if (ImGui::Selectable(settings.name.c_str(), is_selected)) {
                applySettings(settings);
                settings_changed = true;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Color pickers for current settings (if it's 'Custom' or editable)
    if (m_currentSettings.name == "Custom") {
        if (ImGui::ColorEdit3("Screen Background", (float*)&m_currentSettings.screen_background)) settings_changed = true;
        if (ImGui::ColorEdit3("Widget Background", (float*)&m_currentSettings.widget_background)) settings_changed = true;
        if (ImGui::SliderFloat("Corner Roundness", &m_currentSettings.corner_roundness, 0.0f, 12.0f, "%.1f")) settings_changed = true;

        // Font selection
        if (ImGui::BeginCombo("Font", m_currentSettings.font_name.c_str())) {
            for (const auto& fontName : m_availableFontNames) {
                bool is_selected = (m_currentSettings.font_name == fontName);
                if (ImGui::Selectable(fontName.c_str(), is_selected)) {
                    m_currentSettings.font_name = fontName;
                    settings_changed = true;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Font size selection
        if (ImGui::BeginCombo("Font Size", std::to_string(m_currentSettings.font_size).c_str())) {
            for (float fontSize : m_availableFontSizes) {
                bool is_selected = (m_currentSettings.font_size == fontSize);
                if (ImGui::Selectable(std::to_string(fontSize).c_str(), is_selected)) {
                    m_currentSettings.font_size = fontSize;
                    settings_changed = true;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::SliderFloat("UI Scale", &m_currentSettings.scale, 0.5f, 2.0f, "%.1f")) settings_changed = true;
    }

    if (settings_changed) {
        applySettings(m_currentSettings);
    }
}

void SettingsManager::updateAvailableFonts()
{
#ifdef __ANDROID__
    m_availableFontNames = ImGui_ImplAndroid_GetAvailableFontNames();
    m_availableFontSizes = ImGui_ImplAndroid_GetAvailableFontSizes();
#endif
}

void SettingsManager::loadSettingsAsync()
{
    Worker::getInstance().postTask([this]() {
        Settings loadedSettings;
        if (loadSettingsFromState(loadedSettings)) {
            Application::getInstance()->runOnMainThread([this, loadedSettings]() {
                applyLoadedSettings(loadedSettings);
            });
        }
    });
}



