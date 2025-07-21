#pragma once

#include "platform_base.h"
#include <string>
#include <EGL/egl.h>

// Forward declaration
struct ANativeWindow;

// Declare g_initialized as an external variable
extern bool g_initialized;

// Declare EGL globals as extern
extern EGLDisplay g_EglDisplay;
extern EGLSurface g_EglSurface;

// Forward declaration for ImGui Android functions
struct ImDrawData;
extern void ImGui_ImplAndroid_RenderDrawData(ImDrawData* draw_data);

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
    int32_t getScreenOrientation() const; // Added method to get screen orientation
    void* getAndroidApp() override; // Added declaration
    void recreateSwapChain(); // Added declaration
    AAssetManager* getAssetManager() const { return m_assetManager; } // New getter for AssetManager

private:
    // Changed from private to protected to allow access in derived classes
    void* m_androidApp;  // android_app* in actual implementation
    ANativeWindow* m_window = nullptr;  // Store window pointer directly
    ImGuiContext* m_imguiContext = nullptr;  // Store ImGui context pointer
    bool m_keyboardVisible = false;  // Track keyboard visibility
    int m_fbWidth = 0;
    int m_fbHeight = 0;
    AAssetManager* m_assetManager = nullptr; // New member for AssetManager

public:
    int getFramebufferWidth() const override;
    int getFramebufferHeight() const override;
};
