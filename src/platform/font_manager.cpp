#include "../include/platform/font_manager.h"
#include "imgui.h"
#include "../include/platform/logger.h"
#include "../include/platform/state_manager.h" // Include StateManager
#include <string>
#include <vector>
#include <filesystem> // For std::filesystem::path

#if defined(LINUX)
// No-op for Linux
#elif defined(__ANDROID__)
#include "platform/android/imgui_impl_android.h"
#endif

void FontManager::SetDefaultFont(const std::string& fontName, float fontSize)
{
#if defined(LINUX)
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Clear existing fonts

    std::string fontPath = StateManager::getInstance().getInternalDataPath() + "/" + fontName;
    if (std::filesystem::exists(fontPath)) {
        io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
        LOG_INFO("Loaded font: %s (size: %.1f)", fontPath.c_str(), fontSize);
    } else {
        LOG_ERROR("Font file not found: %s", fontPath.c_str());
        // Fallback to default ImGui font if specified font not found
        io.Fonts->AddFontDefault();
    }
    io.Fonts->Build();

#elif defined(__ANDROID__)
    SetDefaultImGuiFont(fontName, fontSize);
#else
    // For other platforms, implement font switching logic here
    LOG_WARN("SetDefaultFont not implemented for this platform.");
#endif
}
