#include "../include/application.h"
#include "../include/logger.h"
#include "../include/scaling_manager.h"
#include "../include/platform_http_client.hpp" // Added for PlatformHttpClient
#include "../include/state_manager.h"
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // Font Awesome icons
#include "imgui.h"
#include <iostream>

// Initialize static instance
Application* Application::s_instance = nullptr;

Application::Application(const std::string& appName, LogWidget* logWidget)
    : m_appName(appName)
    , m_imguiContext(nullptr)
    , m_running(false)
    , m_show_log_widget(true) // Initialize to true for debugging
    , m_log_widget(logWidget) // Initialize with passed pointer
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

    // Main window for the application template
    const char* windowName = "C++ Application Template";
    float x, y;
    if (StateManager::getInstance().loadWindowPosition(windowName, x, y)) {
        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
    }

    ImGui::Begin(windowName, nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    // Save window position
    ImVec2 pos = ImGui::GetWindowPos();
    StateManager::getInstance().saveWindowPosition(windowName, pos.x, pos.y);

    // General information and instructions
    ImGui::Text("Welcome to your cross-platform application!");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "This is a template demonstrating ImGui features.");
    ImGui::Separator();

    // --- Basic Widgets Demonstration ---

    // Text input field
    static char textBuffer[256] = "Hello, world!";
    ImGui::Text("Enter some text below:");
    if (ImGui::InputText("##TextInput", textBuffer, IM_ARRAYSIZE(textBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        // You can add logic here for when the user presses Enter
    }
    ImGui::Text("You entered: %s", textBuffer);

    ImGui::Spacing();

    // Button and counter
    static int buttonClickCount = 0;
    if (ImGui::Button("Click Me")) {
        buttonClickCount++;
    }
    ImGui::SameLine();
    ImGui::Text("Button has been clicked %d times.", buttonClickCount);

    // Font Awesome Icon Button
    if (ImGui::Button(ICON_FA_STAR " Star Button")) {
        
    }

    ImGui::Separator();

    // --- Advanced Widgets Demonstration ---

    if (ImGui::CollapsingHeader("More Features")) {
        // Checkbox for a boolean option
        static bool showExtraInfo = false;
        ImGui::Checkbox("Show Extra Information", &showExtraInfo);
        if (showExtraInfo) {
            ImGui::Text("Here is some extra information, just for you!");
            ImGui::Text("You can hide this by unchecking the box.");
        }

        ImGui::Spacing();

        // Slider for a floating-point value
        static float sliderValue = 0.5f;
        ImGui::SliderFloat("Value Slider", &sliderValue, 0.0f, 1.0f);
        ImGui::Text("Current slider value: %.2f", sliderValue);

        ImGui::Spacing();

        // Color editor
        static ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color Picker", (float*)&color);
        ImGui::TextColored(color, "This text changes color!");
    }

    ImGui::Separator();

    // --- HTTP GET Demonstration ---
    if (ImGui::CollapsingHeader("HTTP GET Demo")) {
        static char urlBuffer[256] = "https://www.google.com";
        static std::string httpGetResponse = "No request made yet.";
        static bool httpRequestRunning = false;

        ImGui::InputText("URL", urlBuffer, IM_ARRAYSIZE(urlBuffer));

        if (ImGui::Button("Send HTTP GET Request") && !httpRequestRunning) {
            httpRequestRunning = true;
            httpGetResponse = "Requesting...";
            // Start the worker in a lambda
            m_httpWorker.start([this, url = std::string(urlBuffer)]() {
                return m_httpClient->get(url, {}, {});
            });
        }
        ImGui::SameLine();
        if (httpRequestRunning) {
            ImGui::Text("Request in progress...");
        } else {
            ImGui::Text("Ready.");
        }

        // Check if the worker has finished
        if (httpRequestRunning && !m_httpWorker.is_running()) {
            HttpResponse response = m_httpWorker.get();
            httpGetResponse = "Status: " + std::to_string(response.status_code) + "\nBody: " + response.text.substr(0, 500) + "..."; // Limit body to 500 chars
            httpRequestRunning = false;
        }

        ImGui::TextWrapped("%s", httpGetResponse.c_str());
    }

    ImGui::Separator();

    // Log Widget Button
    if (ImGui::Button("Toggle Log Window")) {
        m_show_log_widget = !m_show_log_widget;
    }

    ImGui::End();
    
    // Render Log Widget if visible
    if (m_show_log_widget && m_log_widget) {
        m_log_widget->Draw("Application Log", &m_show_log_widget);
    }
    
    // Pop the font
    ImGui::PopFont();

    // New window for "test" label at (0,0)
    const SystemInsets& insets = ScalingManager::getInstance().getSystemInsets();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Display Size: %.1f x %.1f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Text("Framebuffer Size: %d x %d", Application::getInstance()->getFramebufferWidth(), Application::getInstance()->getFramebufferHeight());
    ImGui::End();
}
#endif // USE_EXTERNAL_RENDER_IMGUI
