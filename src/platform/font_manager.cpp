#include "../include/platform/font_manager.h"
#include "imgui.h"
#include "../include/platform/logger.h"
#include "../include/platform/state_manager.h" // Include StateManager
#include <string>
#include <vector>
#include <filesystem> // For std::filesystem::path
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // For Font Awesome icons

#if defined(LINUX)
#include <map>
static std::map<std::string, ImFont*> s_loadedFonts;
#elif defined(__ANDROID__)
#include "platform/android/imgui_impl_android.h"
#endif

void FontManager::SetDefaultFont(const std::string& fontName, float fontSize)
{
#if defined(LINUX)
    ImGuiIO& io = ImGui::GetIO();
    std::string fontKey = fontName + "_" + std::to_string(static_cast<int>(fontSize));
    if (s_loadedFonts.count(fontKey)) {
        io.FontDefault = s_loadedFonts[fontKey];
        LOG_INFO("Switched ImGui font to: %s at size %.1f", fontName.c_str(), fontSize);
    } else {
        LOG_ERROR("Requested font not found: %s at size %.1f. Using default.", fontName.c_str(), fontSize);
        if (!io.Fonts->Fonts.empty()) {
            io.FontDefault = io.Fonts->Fonts[0]; // Fallback to first loaded font
        }
    }
#elif defined(__ANDROID__)
    SetDefaultImGuiFont(fontName, fontSize);
#else
    // For other platforms, implement font switching logic here
    LOG_WARN("SetDefaultFont not implemented for this platform.");
#endif
}

void FontManager::LoadFonts() {
#if defined(LINUX)
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    // Get the internal data path
    std::string dataPath = StateManager::getInstance().getInternalDataPath();

    // List of regular fonts to load
    const char* fontNames[] = {
        "DroidSans.ttf",
        "Cousine-Regular.ttf",
        "Karla-Regular.ttf",
        "ProggyClean.ttf",
        "ProggyTiny.ttf",
        "Roboto-Medium.ttf"
    };

    // Font sizes to load
    const float fontSizes[] = {12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f};

    // Load each regular font and merge the icon font into it for each available size
    for (const char* fontName : fontNames) {
        for (float fontSize : fontSizes) {
            std::string fontPath = dataPath + "/" + fontName;
            if (std::filesystem::exists(fontPath)) {
                ImFontConfig font_cfg;
                font_cfg.FontDataOwnedByAtlas = true;
                ImFont* loadedFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize, &font_cfg);
                if (loadedFont) {
                    s_loadedFonts[std::string(fontName) + "_" + std::to_string(static_cast<int>(fontSize))] = loadedFont;
                    LOG_INFO("Loaded %s at size %.1f", fontName, fontSize);

                    // Merge Font Awesome icons
                    ImFontConfig config;
                    config.MergeMode = true;
                    config.PixelSnapH = true;
                    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
                    std::string faPath = dataPath + "/" + FONT_ICON_FILE_NAME_FAS;
                    if (std::filesystem::exists(faPath)) {
                        io.Fonts->AddFontFromFileTTF(faPath.c_str(), fontSize, &config, icons_ranges);
                    }
                } else {
                    LOG_ERROR("Failed to load %s at size %.1f", fontName, fontSize);
                }
            } else {
                LOG_ERROR("Font file not found: %s", fontPath.c_str());
            }
        }
    }

    // If no fonts could be loaded, add the default font as a fallback.
    if (io.Fonts->Fonts.empty()) {
        io.Fonts->AddFontDefault();
    }

    io.Fonts->Build();
#endif
}
