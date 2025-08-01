#include "../../../../include/platform/platform_font_utils.h"
#include <vector>
#include <string>

// Static vectors to store available font names and sizes
static std::vector<std::string> s_availableFontNames;
static std::vector<float> s_availableFontSizes;

// Functions to get available font names and sizes
const std::vector<std::string>& Platform_GetAvailableFontNames() {
    // This is a placeholder. In a real app, you'd populate this from the system.
    if (s_availableFontNames.empty()) {
        s_availableFontNames = {
            "DroidSans.ttf",
            "Cousine-Regular.ttf",
            "Karla-Regular.ttf",
            "ProggyClean.ttf",
            "ProggyTiny.ttf",
            "Roboto-Medium.ttf"
        };
    }
    return s_availableFontNames;
}

const std::vector<float>& Platform_GetAvailableFontSizes() {
    // This is a placeholder. In a real app, you'd populate this from the system.
    if (s_availableFontSizes.empty()) {
        s_availableFontSizes = {12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f};
    }
    return s_availableFontSizes;
}
