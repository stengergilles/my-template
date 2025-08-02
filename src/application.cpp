#include "../include/application.h"
#include "../include/platform/logger.h"
#include "../include/platform/scaling_manager.h"
#include "../include/platform/platform_http_client.hpp" // Added for PlatformHttpClient
#include "../include/platform/state_manager.h"
#include "../include/platform/settings_manager.h" // Include SettingsManager
#include "../external/IconFontCppHeaders/IconsFontAwesome6.h" // Font Awesome icons
#include "imgui.h"
#include "layout/Layout.h"

#include <iostream>

// Initialize static instance
Application* Application::s_instance = nullptr;

Application::Application(const std::string& appName, LogWidget* logWidget)
    : m_appName(appName)
    , m_imguiContext(nullptr)
    , m_running(false)
    , m_show_log_widget(true) // Initialize to true for debugging
    , m_log_widget(logWidget) // Initialize with passed pointer
    
    , m_currentPage(Page::Home) // Initialize current page to Home
    , m_httpGetResponse("") // Initialize HTTP GET response string
    , m_statusBarMessage("Status: Ready") // Initialize status bar message
{
    // Set singleton instance
    s_instance = this;
    

    // Initialize HTTP client
    m_httpClient = std::make_unique<PlatformHttpClient>();

    // State loading is handled externally after path is set

    // Load current page from state
    std::string pageStr;
    if (StateManager::getInstance().loadString("current_page", pageStr)) {
        if (pageStr == "Home") {
            m_currentPage = Page::Home;
        } else if (pageStr == "SettingsEditor") {
            m_currentPage = Page::SettingsEditor;
        } else if (pageStr == "HttpGetDemo") {
            m_currentPage = Page::HttpGetDemo;
        }
    }
}

Application::~Application()
{
    LOG_INFO("Application destructor called.");
    // Save current page to state
    std::string pageStr;
    switch (m_currentPage) {
        case Page::Home:
            pageStr = "Home";
            break;
        case Page::SettingsEditor:
            pageStr = "SettingsEditor";
            break;
        case Page::HttpGetDemo:
            pageStr = "HttpGetDemo";
            break;
    }
    StateManager::getInstance().saveString("current_page", pageStr);

    // Save application state
    StateManager::getInstance().saveStateAsync();

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

    // Initialize the SettingsManager
    SettingsManager::getInstance().initialize();

    // A default style is applied here.
    // Saved settings will be loaded and applied in the platform-specific init function
    // after platform-dependent resources (like fonts) are loaded.
    ScalingManager::getInstance().setScaleAdjustment(1.0f);
    

    
    
    return true;
}

void Application::runOnMainThread(std::function<void()> task)
{
    std::unique_lock<std::mutex> lock(m_mainThreadMutex);
    m_mainThreadTasks.push(std::move(task));
}

void Application::processMainThreadTasks()
{
    std::function<void()> task;
    while (true) {
        std::unique_lock<std::mutex> lock(m_mainThreadMutex);
        if (m_mainThreadTasks.empty()) {
            break;
        }
        task = m_mainThreadTasks.front();
        m_mainThreadTasks.pop();
        lock.unlock();
        task();
    }
}

int Application::getOrientation() const {
    if (ImGui::GetIO().DisplaySize.y > ImGui::GetIO().DisplaySize.x) {
        return 1; // Portrait
    } else {
        return 0; // Landscape
    }
}



void Application::renderFrame()
{
    // Set clear color based on settings
    ImVec4 clear_color = SettingsManager::getInstance().getScreenBackground();
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(0, 0),
        ImGui::GetIO().DisplaySize,
        IM_COL32(clear_color.x * 255, clear_color.y * 255, clear_color.z * 255, clear_color.w * 255)
    );

    // Start a new frame
    platformNewFrame();
    ImGui::NewFrame();

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
    if (Application::getInstance()->getOrientation() == 1) // 1 for portrait
    {
        centerLeftWidth = {SizeMode::PERCENTAGE, 30.0f};
    }
    Layout::BeginCard(
        "CenterLeftOptions",
        centerLeftWidth,
        {SizeMode::AUTOFIT, 0.0f},
        HAlignment::LEFT,
        VAlignment::CENTER,
        []() {
            ImGui::Text("Navigation:");
            if (ImGui::Button("Home")) {
                Application::getInstance()->m_currentPage = Application::Page::Home;
                StateManager::getInstance().saveString("current_page", "Home");
            }
            if (ImGui::Button("Settings Editor")) {
                Application::getInstance()->m_currentPage = Application::Page::SettingsEditor;
                StateManager::getInstance().saveString("current_page", "SettingsEditor");
            }
            if (ImGui::Button("HTTP GET Demo")) {
                Application::getInstance()->m_currentPage = Application::Page::HttpGetDemo;
                StateManager::getInstance().saveString("current_page", "HttpGetDemo");
            }
            ImGui::Separator();
            static bool option1 = false;
            static int radio = 0;
            ImGui::Text("Options:");
            ImGui::Checkbox("Option 1", &option1);
            ImGui::RadioButton("Radio 1", &radio, 0);
            if (Application::getInstance()->getOrientation() != 1) // Not portrait
            ImGui::SameLine();
            ImGui::RadioButton("Radio 2", &radio, 1);
            if (Application::getInstance()->getOrientation() != 1) // Not portrait
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
        [this]() {
            // Render the current page
            switch (m_currentPage) {
                case Page::Home:
                    renderHomePage();
                    break;
                case Page::SettingsEditor:
                    SettingsManager::getInstance().showSettingsEditor();
                    break;
                case Page::HttpGetDemo:
                    renderHttpGetDemoPage();
                    break;
            }
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
            Application* app = Application::getInstance();
            if (app) {
                ImGui::Text("%s", app->m_statusBarMessage.c_str());
            }
        }
    );
    Layout::EndCard();

    // End the custom card layout, triggering calculation and rendering
    Layout::EndCardLayout(ImGui::GetIO().DisplaySize);

    // Render Log Widget if visible
    if (m_log_widget) {
        m_log_widget->Draw("Application Log", NULL);
    }
    
    // Render application frame
    // renderImGui(); // Removed as content is now in CenterContent card
    
    // Render and present
    ImGui::Render();
    platformRender();
}

#ifndef USE_EXTERNAL_RENDER_IMGUI
// Removed renderImGui function as its content is now part of renderFrame

void Application::renderHomePage()
{
    ImGui::Text("Welcome to the Home Page!");
    ImGui::Text("Use the navigation on the left to explore.");
}

void Application::renderHttpGetDemoPage()
{
    ImGui::Text("HTTP GET Demo Page");
    ImGui::Separator();
    static char urlBuffer[256] = "https://www.google.com";
    ImGui::InputText("URL", urlBuffer, sizeof(urlBuffer));
    if (ImGui::Button("Send GET Request")) {
        Application* app = Application::getInstance();
        if (app) {
            app->m_statusBarMessage = "Status: Sending request...";
            Worker::getInstance().postTask([app, url = std::string(urlBuffer)]() {
                HttpResponse response = app->m_httpClient->get(url, {}, {});
                app->runOnMainThread([app, response]() {
                    if (response.status_code == 200) {
                        app->m_httpGetResponse = response.text;
                        app->m_statusBarMessage = "Status: Request successful!";
                    } else {
                        app->m_httpGetResponse = "Error: " + std::to_string(response.status_code) + " - " + response.text;
                        app->m_statusBarMessage = "Status: Request failed with error " + std::to_string(response.status_code);
                    }
                });
            });
        }
        LOG_INFO("Sending GET request to: %s", urlBuffer);
    }
    ImGui::Text("Response:");
    ImGui::BeginChild("##http_response", ImVec2(0, 0), true);
    ImGui::TextWrapped("%s", m_httpGetResponse.c_str());
    ImGui::EndChild();
}
#endif // USE_EXTERNAL_RENDER_IMGUI
