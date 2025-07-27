#include "../include/font_manager.h"
#include "imgui.h"
#include "../include/platform/logger.h"

#ifdef __ANDROID__
#include "platform/android/imgui_impl_android.h"
#endif

void FontManager::SetDefaultFont(const std::string& fontName, float fontSize)
{
#ifdef __ANDROID__
    SetDefaultImGuiFont(fontName, fontSize);
#else
    // For other platforms, implement font switching logic here
    LOG_WARN("SetDefaultFont not implemented for this platform.");
#endif
}
