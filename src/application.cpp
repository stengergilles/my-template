#include "../include/application.h"
#include "../include/logger.h"
#include "../include/scaling_manager.h"
#include "../include/platform_http_client.hpp" // Added for PlatformHttpClient
#include "../include/state_manager.h"
#include "../include/theme_manager.h" // Include ThemeManager
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // Font Awesome icons
#include "imgui.h"
#include "layout/Layout.h"
#include "platform/platform_android.h" // Include for PlatformAndroid
#include <iostream>

// Initialize static instance
Application* Application::s_instance = nullptr;

Application::Application(const std::string& appName, LogWidget* logWidget)
    : m_appName(appName)
    , m_imguiContext(nullptr)
    , m_running(false)
    , m_show_log_widget(true) // Initialize to true for debugging
    , m_log_widget(logWidget) // Initialize with passed pointer
    , m_themeManager() // Initialize ThemeManager
{
    // Set singleton instance
    s_instance = this;
    

    // Initialize HTTP client
    m_httpClient = std::make_unique<PlatformHttpClient>();

    // Load application state
    StateManager::getInstance().loadState();
}

Application::~Application()
{
    // Save application state
    StateManager::getInstance().saveState();

    // ImGui cleanup is handled in platformShutdown()
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool Application::initImGui()
{
    // Create ImGui context
    m_imguiContext = ImGui::CreateContext();
    if (!m_imguiContext) {
        LOG_ERROR("Failed to create ImGui context");
        return false;
    }

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Apply default theme
    m_themeManager.applyTheme(m_themeManager.getAvailableThemes()[0]);

    // Load fonts
    m_themeManager.loadFonts();
    
    return true;
}

void Application::run()
{
    // For Android, we don't run the main loop here
    // The main loop is handled in android_main.cpp
    #ifndef __ANDROID__
    // Initialize ImGui first
    if (!initImGui()) {
        LOG_ERROR("ImGui initialization failed");
        return;
    }

    // Initialize platform-specific components
    if (!platformInit()) {
        LOG_ERROR("Platform initialization failed");
        return;
    }

    // Main loop
    m_running = true;
    while (m_running) {
        // Handle platform events (may set m_running to false)
        m_running = platformHandleEvents();
        
        // Render a frame
        renderFrame();
    }

    // Cleanup
    platformShutdown();
    #endif
}

void Application::renderFrame()
{
    // Start a new frame
    platformNewFrame();
    ImGui::NewFrame();

    // Set clear color based on theme
    ImVec4 clear_color = m_themeManager.getScreenBackground();
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(0, 0),
        ImGui::GetIO().DisplaySize,
        IM_COL32(clear_color.x * 255, clear_color.y * 255, clear_color.z * 255, clear_color.w * 255)
    );

    // Begin the custom card layout
    Layout::BeginCardLayout();

    // Define the cards
    using namespace Layout;

    // Top-left widget: 100% width, content height, toolbar aligned right
    Layout::BeginCard(
        "TopLeftToolbar",
        {SizeMode::PERCENTAGE, 100.0f},
        {SizeMode::CONTENT, 0.0f},
        HAlignment::LEFT,
        VAlignment::TOP,
        []() {
            ImGui::Text("My Application");
            ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetFrameHeight() * 3 + ImGui::GetStyle().ItemSpacing.x * 2 + ImGui::GetStyle().WindowPadding.x)); // Align right with padding
            if (ImGui::Button(ICON_FA_FOLDER_OPEN)) { /* Open */ }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_FLOPPY_DISK)) { /* Save */ }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_GEAR)) { /* Settings */ }
        }
    );
    Layout::EndCard();

    // Center-left widget: content width (or percentage in portrait), autofit height, options and buttons
    Dimension centerLeftWidth = {SizeMode::CONTENT, 0.0f};
#ifdef __ANDROID__
    if (static_cast<PlatformAndroid*>(Application::getInstance())->getScreenOrientation() == 1) // 1 for ACONFIGURATION_ORIENTATION_PORT
    {
        centerLeftWidth = {SizeMode::PERCENTAGE, 30.0f};
    }
#endif
    Layout::BeginCard(
        "CenterLeftOptions",
        centerLeftWidth,
        {SizeMode::AUTOFIT, 0.0f},
        HAlignment::LEFT,
        VAlignment::CENTER,
        []() {
            static bool option1 = false;
            static int radio = 0;
            ImGui::Text("Options:");
            ImGui::Checkbox("Option 1", &option1);
            ImGui::RadioButton("Radio 1", &radio, 0);
#ifdef __ANDROID__
            if (static_cast<PlatformAndroid*>(Application::getInstance())->getScreenOrientation() != 1) // Not portrait
#endif
            ImGui::SameLine();
            ImGui::RadioButton("Radio 2", &radio, 1);
#ifdef __ANDROID__
            if (static_cast<PlatformAndroid*>(Application::getInstance())->getScreenOrientation() != 1) // Not portrait
#endif
            ImGui::SameLine();
            ImGui::RadioButton("Radio 3", &radio, 2);
            ImGui::Separator();
            if (ImGui::Button("Action 1")) { /* Do something */ }
            if (ImGui::Button("Action 2")) { /* Do something else */ }
        },
        true // Make this card hideable
    );
    Layout::EndCard();

    // Center widget: autofit width, autofit height
    Layout::BeginCard(
        "CenterContent",
        {SizeMode::AUTOFIT, 0.0f},
        {SizeMode::AUTOFIT, 0.0f},
        HAlignment::CENTER,
        VAlignment::CENTER,
        []() {
            ImGui::Text("Main content area.");
            ImGui::Text("This widget should fill the remaining space.");
        }
    );
    Layout::EndCard();

    // Bottom widget: autofit width, content height, status bar aligned left
    Layout::BeginCard(
        "BottomStatusBar",
        {SizeMode::AUTOFIT, 0.0f},
        {SizeMode::CONTENT, 0.0f},
        HAlignment::LEFT,
        VAlignment::BOTTOM,
        []() {
            ImGui::Text("Status: Ready");
        }
    );
    Layout::EndCard();

    // End the custom card layout, triggering calculation and rendering
    Layout::EndCardLayout(ImGui::GetIO().DisplaySize);
    
    // Render application frame
    renderImGui();
    
    // Render and present
    ImGui::Render();
    platformRender();
}

#ifndef USE_EXTERNAL_RENDER_IMGUI
void Application::renderImGui()
{
    // Push the 12px font
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    // Render Log Widget if visible
    if (m_log_widget) {
        m_log_widget->Draw("Application Log", NULL);
    }

    // Show Theme Editor
    m_themeManager.showThemeEditor();
    
    // Pop the font
    ImGui::PopFont();
}
#endif // USE_EXTERNAL_RENDER_IMGUI
