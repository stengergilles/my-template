#include "../include/application.h"
#include "../include/logger.h"
#include "../include/scaling_manager.h"
#include "imgui.h"
#include <iostream>

// Initialize static instance
Application* Application::s_instance = nullptr;

Application::Application(const std::string& appName)
    : m_appName(appName)
    , m_imguiContext(nullptr)
    , m_running(false)
{
    // Set singleton instance
    s_instance = this;
    LOG_INFO("Application created: %s", m_appName.c_str());
}

Application::~Application()
{
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
    // Initialize platform-specific components
    if (!platformInit()) {
        LOG_ERROR("Platform initialization failed");
        return;
    }

    // Initialize ImGui
    if (!initImGui()) {
        LOG_ERROR("ImGui initialization failed");
        platformShutdown();
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
    // Main window for the application template
    ImGui::Begin("C++ Application Template");

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

    ImGui::End();
    
    // Close the SafeAreaConstraint child window if it was opened
    #ifdef __ANDROID__
    if (ImGui::GetIO().UserData) {
        ImGui::End(); // End the SafeAreaViewport window
        ImGui::GetIO().UserData = nullptr; // Reset the flag
    }
    #endif
}
#endif // USE_EXTERNAL_RENDER_IMGUI
