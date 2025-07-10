#pragma once

#include <string>
#include <memory> // For std::unique_ptr

#include "http_client.hpp"
#include "worker.hpp"
#include "platform_http_client.hpp" // For createPlatformHttpClient()
#include "log_widget.h"

// Forward declarations
struct ImGuiContext;
class PlatformBase;

class Application {
public:
    Application(const std::string& appName = "ImGui Hello World", LogWidget* logWidget = nullptr);
    virtual ~Application();

    // Initialize ImGui (platform-independent)
    bool initImGui();
    
    // Main application loop
    void run();
    
    // Render a frame
    void renderFrame();

    // Getters
    const std::string& getAppName() const { return m_appName; }
    
    // Get platform instance - commented out to fix circular dependency
    // PlatformBase* getPlatform() { return dynamic_cast<PlatformBase*>(this); }
    
    // Singleton access
    static Application* getInstance() { return s_instance; }

protected:
    // Platform-specific implementations (to be overridden by platform classes)
    virtual bool platformInit() = 0;
    virtual void platformShutdown() = 0;
    virtual void platformNewFrame() = 0;
    virtual void platformRender() = 0;
    virtual bool platformHandleEvents() = 0;

    std::string m_appName;
    ImGuiContext* m_imguiContext;
    bool m_running;
    bool m_show_log_widget; // Moved from Application.cpp
    LogWidget* m_log_widget; // Log widget instance
    std::unique_ptr<HttpClient> m_httpClient;
    Worker<HttpResponse> m_httpWorker;

private:
    // ImGui rendering code (platform-independent)
    void renderImGui();

    static Application* s_instance; // Singleton instance
};
