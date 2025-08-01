#include "../include/platform/font_manager.h"
#include "imgui.h"
#include "../include/platform/logger.h"

#if defined(LINUX)
// No-op for Linux
#elif defined(__ANDROID__)
#include "platform/android/imgui_impl_android.h"
#endif

void FontManager::SetDefaultFont(const std::string& fontName, float fontSize)
{
#if defined(LINUX)
    // For other platforms, implement font switching logic here
    LOG_WARN("SetDefaultFont not implemented for this platform.");
#elif defined(__ANDROID__)
    SetDefaultImGuiFont(fontName, fontSize);
#else
    // For other platforms, implement font switching logic here
    LOG_WARN("SetDefaultFont not implemented for this platform.");
#endif
}
