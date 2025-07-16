#pragma once

#include "platform_base.h"
#include <string>

// Forward declaration
struct ANativeWindow;

// Declare g_initialized as an external variable
extern bool g_initialized;

class PlatformAndroid : public PlatformBase {
public:
    PlatformAndroid(const std::string& title, LogWidget* logWidget);
    virtual ~PlatformAndroid();
    
    virtual bool platformInit() override;
    virtual void platformNewFrame() override;
    virtual void platformRender() override;
    virtual bool platformHandleEvents() override;
    virtual void platformShutdown() override;  // Implement the pure virtual method
    
    // Add a public setter method for m_androidApp
    void setAndroidApp(void* app);
    
    // Add a direct window initialization method
    bool initWithWindow(ANativeWindow* window);
    
    // Override the getAndroidApp method from PlatformBase
    virtual void* getAndroidApp() override;

    void recreateSwapChain();

protected:
    // Changed from private to protected to allow access in derived classes
    void* m_androidApp;  // android_app* in actual implementation
    ANativeWindow* m_window = nullptr;  // Store window pointer directly
    ImGuiContext* m_imguiContext = nullptr;  // Store ImGui context pointer
    bool m_keyboardVisible = false;  // Track keyboard visibility
};
