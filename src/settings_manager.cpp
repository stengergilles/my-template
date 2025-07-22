#include "../include/settings_manager.h"
#include "../include/logger.h"
#include "../include/state_manager.h"
#include "../include/scaling_manager.h"
#include "imgui.h"
#include "imgui_internal.h" // For ImGui::GetStyle()
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // For ICON_MIN_FA, ICON_MAX_FA

#ifndef __ANDROID__
#include "../include/platform/linux/asset_manager.h"
#endif

SettingsManager::SettingsManager()
{
    setupDefaultSettings();
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
    darkSettings.font_name = "DroidSans.ttf"; // Default font
    darkSettings.font_size = 12.0f; // Default font size
    darkSettings.scale = 1.0f; // Default scale
    m_availableSettings.push_back(darkSettings);

    // Light Settings
    Settings lightSettings;
    lightSettings.name = "Light";
    lightSettings.screen_background = ImVec4(0.6f, 0.7f, 1.0f, 1.0f); // Light blue
    lightSettings.widget_background = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Slightly darker grey
    lightSettings.corner_roundness = 0.0f; // No roundness for light settings
    lightSettings.font_name = "DroidSans.ttf"; // Default font
    lightSettings.font_size = 12.0f; // Default font size
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

    // Apply selected font
    std::string fontKey = m_currentSettings.font_name + "_" + std::to_string(static_cast<int>(m_currentSettings.font_size));
    if (m_fonts.count(fontKey)) {
        ImGui::GetIO().FontDefault = m_fonts[fontKey];
        LOG_INFO("Applied font: %s at size %.1f", m_currentSettings.font_name.c_str(), m_currentSettings.font_size);
    } else {
        LOG_ERROR("Font not found: %s at size %.1f. Using default.", m_currentSettings.font_name.c_str(), m_currentSettings.font_size);
        ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[0]; // Fallback to first loaded font
    }

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
}

bool SettingsManager::loadSettingsFromState()
{
    std::string settingsName;
    if (StateManager::getInstance().loadString("settings_name", settingsName)) {
        Settings loadedSettings;
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

        applySettings(loadedSettings);
        ScalingManager::getInstance().setScaleAdjustment(loadedSettings.scale);
        LOG_INFO("Loaded settings from state: %s", settingsName.c_str());
        return true;
    }
    return false;
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
    // Settings selection dropdown
    if (ImGui::BeginCombo("Select Settings", m_currentSettings.name.c_str())) {
        for (const auto& settings : m_availableSettings) {
            bool is_selected = (m_currentSettings.name == settings.name);
            if (ImGui::Selectable(settings.name.c_str(), is_selected)) {
                applySettings(settings);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Color pickers for current settings (if it's 'Custom' or editable)
    if (m_currentSettings.name == "Custom") {
        ImGui::ColorEdit3("Screen Background", (float*)&m_currentSettings.screen_background);
        ImGui::ColorEdit3("Widget Background", (float*)&m_currentSettings.widget_background);
        ImGui::SliderFloat("Corner Roundness", &m_currentSettings.corner_roundness, 0.0f, 12.0f, "%.1f");

        // Font selection
        if (ImGui::BeginCombo("Font", m_currentSettings.font_name.c_str())) {
            for (const auto& fontName : m_availableFontNames) {
                bool is_selected = (m_currentSettings.font_name == fontName);
                if (ImGui::Selectable(fontName.c_str(), is_selected)) {
                    m_currentSettings.font_name = fontName;
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
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Apply Custom Settings")) {
            applySettings(m_currentSettings);
        }

        ImGui::SliderFloat("UI Scale", &m_currentSettings.scale, 0.5f, 2.0f, "%.1f");
    }
}

#ifdef __ANDROID__
void SettingsManager::loadFonts(AAssetManager* assetManager)
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Clear existing fonts

    m_availableFontNames = {
        "Cousine-Regular.ttf",
        "DroidSans.ttf",
        "fa-solid-900.ttf",
        "Karla-Regular.ttf",
        "ProggyClean.ttf",
        "ProggyTiny.ttf",
        "Roboto-Medium.ttf"
    };

    m_availableFontSizes = {12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f};

    // Load fonts from assets
    for (const auto& fontName : m_availableFontNames) {
        AAsset* asset = AAssetManager_open(assetManager, fontName.c_str(), AASSET_MODE_BUFFER);
        if (asset) {
            size_t file_size = AAsset_getLength(asset);
            std::vector<char> font_data(file_size);
            AAsset_read(asset, font_data.data(), file_size);
            AAsset_close(asset);

            // Store font data to ensure it stays in memory
            m_fontData[fontName] = font_data;

            for (float fontSize : m_availableFontSizes) {
                ImFontConfig font_cfg;
                font_cfg.FontDataOwnedByAtlas = false; // We own the data
                ImFont* font = io.Fonts->AddFontFromMemoryTTF(m_fontData[fontName].data(), m_fontData[fontName].size(), fontSize, &font_cfg);
                if (font) {
                    m_fonts[fontName + "_" + std::to_string(static_cast<int>(fontSize))] = font;
                    LOG_INFO("Loaded font: %s at size %.1f", fontName.c_str(), fontSize);
                } else {
                    LOG_ERROR("Failed to load font: %s at size %.1f", fontName.c_str(), fontSize);
                }
            }
        } else {
            LOG_ERROR("Failed to open font asset: %s", fontName.c_str());
        }
    }

    // Merge icon font into default font
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    AAsset* icon_asset = AAssetManager_open(assetManager, "fa-solid-900.ttf", AASSET_MODE_BUFFER);
    if (icon_asset) {
        size_t file_size = AAsset_getLength(icon_asset);
        std::vector<char> icon_font_data(file_size);
        AAsset_read(icon_asset, icon_font_data.data(), file_size);
        AAsset_close(icon_asset);
        m_fontData["fa-solid-900.ttf"] = icon_font_data; // Store icon font data
        io.Fonts->AddFontFromMemoryTTF(m_fontData["fa-solid-900.ttf"].data(), m_fontData["fa-solid-900.ttf"].size(), 12.0f, &config, icon_ranges);
    } else {
        LOG_ERROR("Failed to open icon font asset: fa-solid-900.ttf");
    }

    io.Fonts->Build();
}
#else
#include <libgen.h> // For dirname
#include <limits.h> // For PATH_MAX
#include <unistd.h> // For readlink

void SettingsManager::loadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Clear existing fonts

    m_availableFontNames = {
        "Cousine-Regular.ttf",
        "DroidSans.ttf",
        "fa-solid-900.ttf",
        "Karla-Regular.ttf",
        "ProggyClean.ttf",
        "ProggyTiny.ttf",
        "Roboto-Medium.ttf"
    };

    m_availableFontSizes = {12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f};

    // Determine executable path and construct assets/fonts path
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    std::string executablePath;
    if (count != -1) {
        executablePath = std::string(path, (size_t)count);
    } else {
        LOG_ERROR("Failed to get executable path.");
        return;
    }

    std::string executableDir = dirname(const_cast<char*>(executablePath.c_str()));
    std::string fontBasePath = executableDir + "/assets/fonts";

    LinuxAssetManager assetManager(fontBasePath);

    // Load fonts using LinuxAssetManager
    for (const auto& fontName : m_availableFontNames) {
        std::shared_ptr<LinuxAsset> asset = assetManager.open(fontName);
        if (asset && asset->getBuffer()) {
            // Store font data to ensure it stays in memory
            m_fontData[fontName] = asset;

            for (float fontSize : m_availableFontSizes) {
                ImFontConfig font_cfg;
                font_cfg.FontDataOwnedByAtlas = false; // We own the data
                ImFont* font = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(asset->getBuffer()), asset->getLength(), fontSize, &font_cfg);
                if (font) {
                    m_fonts[fontName + "_" + std::to_string(static_cast<int>(fontSize))] = font;
                    LOG_INFO("Loaded font: %s at size %.1f", fontName.c_str(), fontSize);
                } else {
                    LOG_ERROR("Failed to load font: %s at size %.1f", fontName.c_str(), fontSize);
                }
            }
        } else {
            LOG_ERROR("Failed to open font asset: %s", fontName.c_str());
        }
    }

    // Merge icon font into default font
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    std::shared_ptr<LinuxAsset> icon_asset = assetManager.open("fa-solid-900.ttf");
    if (icon_asset && icon_asset->getBuffer()) {
        m_fontData["fa-solid-900.ttf"] = icon_asset; // Store icon font data
        io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(icon_asset->getBuffer()), icon_asset->getLength(), 12.0f, &config, icon_ranges);
    } else {
        LOG_ERROR("Failed to open icon font asset: fa-solid-900.ttf");
    }

    io.Fonts->Build();
}
#endif
