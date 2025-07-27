#pragma once

#include <string>
#include <vector>

class FontManager
{
public:
    // Generic function to set the default ImGui font
    static void SetDefaultFont(const std::string& fontName, float fontSize);
};