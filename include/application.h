#pragma once

#include <string>
#include <memory> // For std::unique_ptr

#include "http_client.hpp"
#include "platform/worker.hpp"
#include "platform/platform_http_client.hpp" // For createPlatformHttpClient()
#include "widget/log_widget.h"
#include "layout/Layout.h"
#include "platform/settings_manager.h" // Include SettingsManager for full definition

// Forward declarations
struct ImGuiContext;
class PlatformBase;

class Application {
public:
    Application(const std::string& appName = "ImGui Hello World", LogWidget* logWidget = nullptr);
    virtual ~Application();

    // Initialize ImGui (platform-independent)
    bool initImGui();
    
    
    
    // Render a frame
    void renderFrame();

    // Getters
    const std::string& getAppName() const { return m_appName; }
    
    // Get platform instance - commented out to fix circular dependency
    // PlatformBase* getPlatform() { return dynamic_cast<PlatformBase*>(this); }
    
    // Singleton access
    static Application* getInstance() { return s_instance; }

    void runOnMainThread(std::function<void()> task);

protected:
    // Platform-specific implementations (to be overridden by platform classes)
    virtual bool platformInit() = 0;
    virtual void platformShutdown() = 0;
    virtual void platformNewFrame() = 0;
    virtual void platformRender() = 0;
    virtual bool platformHandleEvents() = 0;

    virtual int getFramebufferWidth() const = 0;
    virtual int getFramebufferHeight() const = 0;

    int getOrientation() const;

    std::string m_appName;
    ImGuiContext* m_imguiContext;
    bool m_running;
    bool m_show_log_widget; // Moved from Application.cpp
    LogWidget* m_log_widget; // Log widget instance
    std::unique_ptr<HttpClient> m_httpClient;
    
         // Add SettingsManager member

    enum class Page {
        Home,
        SettingsEditor,
        HttpGetDemo
    };

    Page m_currentPage; // Current active page
    std::string m_httpGetResponse; // To store HTTP GET response
    std::string m_statusBarMessage; // To store status bar messages

protected:
    std::queue<std::function<void()>> m_mainThreadTasks;
    std::mutex m_mainThreadMutex;
public:
    void processMainThreadTasks();

    void renderHomePage();
    void renderHttpGetDemoPage();

    static Application* s_instance; // Singleton instance
};
