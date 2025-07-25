#pragma once

#include <vector>
#include <string>

// Forward declaration for AAssetManager
struct AAssetManager;

// Function to set the asset manager (defined in imgui_impl_android.cpp)
void ImGui_ImplAndroid_SetAssetManager(AAssetManager* assetManager);

// Functions to get available font names and sizes
const std::vector<std::string>& ImGui_ImplAndroid_GetAvailableFontNames();
const std::vector<float>& ImGui_ImplAndroid_GetAvailableFontSizes();

void SetDefaultImGuiFont(const std::string& fontName, float fontSize);
