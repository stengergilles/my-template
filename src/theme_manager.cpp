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
    darkTheme.corner_roundness = 5.0f; // Default roundness for dark theme
    darkTheme.font_name = "DroidSans.ttf"; // Default font
    darkTheme.font_size = 12.0f; // Default font size
    m_availableThemes.push_back(darkTheme);

    // Light Theme
    Theme lightTheme;
    lightTheme.name = "Light";
    lightTheme.screen_background = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Light grey
    lightTheme.widget_background = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Slightly darker grey
    lightTheme.corner_roundness = 0.0f; // No roundness for light theme
    lightTheme.font_name = "DroidSans.ttf"; // Default font
    lightTheme.font_size = 12.0f; // Default font size
    m_availableThemes.push_back(lightTheme);

    // Custom Theme (example)
    Theme customTheme;
    customTheme.name = "Custom";
    customTheme.screen_background = ImVec4(0.15f, 0.05f, 0.2f, 1.0f); // Purple-ish dark
    customTheme.widget_background = ImVec4(0.3f, 0.1f, 0.4f, 1.0f); // Purple-ish light
    customTheme.corner_roundness = 10.0f; // More roundness for custom theme
    customTheme.font_name = "DroidSans.ttf"; // Default font
    customTheme.font_size = 12.0f; // Default font size
    m_availableThemes.push_back(customTheme);
}

void ThemeManager::applyTheme(const Theme& theme)
{
    m_currentTheme = theme;
    applyImGuiStyle(theme);

    // Apply selected font
    std::string fontKey = m_currentTheme.font_name + "_" + std::to_string(static_cast<int>(m_currentTheme.font_size));
    if (m_fonts.count(fontKey)) {
        ImGui::GetIO().FontDefault = m_fonts[fontKey];
        LOG_INFO("Applied font: %s at size %.1f", m_currentTheme.font_name.c_str(), m_currentTheme.font_size);
    } else {
        LOG_ERROR("Font not found: %s at size %.1f. Using default.", m_currentTheme.font_name.c_str(), m_currentTheme.font_size);
        ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts[0]; // Fallback to first loaded font
    }

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
    style.WindowRounding = theme.corner_roundness;
    style.FrameRounding = theme.corner_roundness;
    style.GrabRounding = theme.corner_roundness;
    style.PopupRounding = theme.corner_roundness;
    style.ScrollbarRounding = theme.corner_roundness;
    style.TabRounding = theme.corner_roundness;
    style.ChildRounding = theme.corner_roundness;

    // Spacing
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
}

void ThemeManager::showThemeEditor()
{
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
        ImGui::SliderFloat("Corner Roundness", &m_currentTheme.corner_roundness, 0.0f, 12.0f, "%.1f");

        // Font selection
        if (ImGui::BeginCombo("Font", m_currentTheme.font_name.c_str())) {
            for (const auto& fontName : m_availableFontNames) {
                bool is_selected = (m_currentTheme.font_name == fontName);
                if (ImGui::Selectable(fontName.c_str(), is_selected)) {
                    m_currentTheme.font_name = fontName;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Font size selection
        if (ImGui::BeginCombo("Font Size", std::to_string(m_currentTheme.font_size).c_str())) {
            for (float fontSize : m_availableFontSizes) {
                bool is_selected = (m_currentTheme.font_size == fontSize);
                if (ImGui::Selectable(std::to_string(fontSize).c_str(), is_selected)) {
                    m_currentTheme.font_size = fontSize;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Apply Custom Theme")) {
            applyTheme(m_currentTheme);
        }
    }
}

void ThemeManager::loadFonts(AAssetManager* assetManager)
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
