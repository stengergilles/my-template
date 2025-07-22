#include "../include/settings_manager.h"
#include "../include/logger.h"
#include "../include/state_manager.h"
#include "imgui.h"
#include "imgui_internal.h" // For ImGui::GetStyle()
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // For ICON_MIN_FA, ICON_MAX_FA

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
    m_availableSettings.push_back(darkSettings);

    // Light Settings
    Settings lightSettings;
    lightSettings.name = "Light";
    lightSettings.screen_background = ImVec4(0.6f, 0.7f, 1.0f, 1.0f); // Light blue
    lightSettings.widget_background = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Slightly darker grey
    lightSettings.corner_roundness = 0.0f; // No roundness for light settings
    lightSettings.font_name = "DroidSans.ttf"; // Default font
    lightSettings.font_size = 12.0f; // Default font size
    m_availableSettings.push_back(lightSettings);

    // Custom Settings (example)
    Settings customSettings;
    customSettings.name = "Custom";
    customSettings.screen_background = ImVec4(0.15f, 0.05f, 0.2f, 1.0f); // Purple-ish dark
    customSettings.widget_background = ImVec4(0.3f, 0.1f, 0.4f, 1.0f); // Purple-ish light
    customSettings.corner_roundness = 10.0f; // More roundness for custom settings
    customSettings.font_name = "DroidSans.ttf"; // Default font
    customSettings.font_size = 12.0f; // Default font size
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

        applySettings(loadedSettings);
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
    }
}

void SettingsManager::loadFonts(AAssetManager* assetManager)
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Clear existing fonts
    m_fontData.clear(); // Clear stored font data

    // Populate available font names and sizes
    m_availableFontNames.clear();
    m_availableFontNames.push_back("DroidSans.ttf");
    m_availableFontNames.push_back("Karla-Regular.ttf");
    m_availableFontNames.push_back("Roboto-Medium.ttf");
    LOG_INFO("Available font names populated. Count: %zu", m_availableFontNames.size());

    m_availableFontSizes.clear();
    m_availableFontSizes.push_back(12.0f);
    m_availableFontSizes.push_back(16.0f);
    m_availableFontSizes.push_back(18.0f);

    // Load specified fonts and sizes
    for (const auto& fontName : m_availableFontNames) {
        for (float fontSize : m_availableFontSizes) {
            std::string fontPath = "external/imgui/misc/fonts/" + fontName;
            std::string fontKey = fontName + "_" + std::to_string(static_cast<int>(fontSize));

#ifdef __ANDROID__
            if (assetManager) {
                AAsset* asset = AAssetManager_open(assetManager, fontName.c_str(), AASSET_MODE_BUFFER);
                if (asset) {
                    size_t file_size = AAsset_getLength(asset);
                    m_fontData[fontKey].resize(file_size);
                    AAsset_read(asset, m_fontData[fontKey].data(), file_size);
                    AAsset_close(asset);

                    ImFont* font = io.Fonts->AddFontFromMemoryTTF(m_fontData[fontKey].data(), file_size, fontSize);
                    if (font) {
                        m_fonts[fontKey] = font;
                        LOG_INFO("Loaded font: %s at size %.1f from assets.", fontName.c_str(), fontSize);
                    } else {
                        LOG_ERROR("Failed to load font: %s at size %.1f from assets.", fontName.c_str(), fontSize);
                    }
                    
                } else {
                    LOG_ERROR("Failed to open font: %s from assets. Asset is null.", fontName.c_str());
                }
            } else {
                LOG_ERROR("AssetManager is null. Cannot load fonts from assets.");
            }
#else
            ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
            if (font) {
                m_fonts[fontKey] = font;
                LOG_INFO("Loaded font: %s at size %.1f.", fontName.c_str(), fontSize);
            } else {
                LOG_ERROR("Failed to load font: %s at size %.1f.", fontName.c_str(), fontSize);
            }
#endif
        }
    }

    // Load FontAwesome font (fa-solid-900.ttf) - 12px
    // This is typically loaded with a merge to the default font for icons.
    // Ensure the path is correct for your setup.
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

#ifdef __ANDROID__
    if (assetManager) {
        AAsset* asset = AAssetManager_open(assetManager, "fa-solid-900.ttf", AASSET_MODE_BUFFER);
        if (asset) {
            size_t file_size = AAsset_getLength(asset);
            m_fontData["FontAwesome"].resize(file_size);
            AAsset_read(asset, m_fontData["FontAwesome"].data(), file_size);
            AAsset_close(asset);

            ImFont* fontAwesome = io.Fonts->AddFontFromMemoryTTF(m_fontData["FontAwesome"].data(), file_size, 12.0f, &config, icon_ranges);
            if (fontAwesome) {
                m_fonts["FontAwesome"] = fontAwesome;
                LOG_INFO("Loaded FontAwesome font: fa-solid-900.ttf from assets.");
            } else {
                LOG_ERROR("Failed to load FontAwesome font: fa-solid-900.ttf from assets.");
            }
        } else {
            LOG_ERROR("Failed to open fa-solid-900.ttf from assets.");
        }
    } else {
        LOG_ERROR("AssetManager is null. Cannot load FontAwesome font from assets.");
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
