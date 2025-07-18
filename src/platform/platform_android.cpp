#include <android/configuration.h>  // Added for AConfiguration_getScreenDensity
#include <android/native_activity.h>
#include <android/input.h>
#include <android/looper.h>  // For ALooper_pollAll

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "imgui.h"
#include "../include/platform/platform_android.h"
#include "../../include/logger.h"

// Include keyboard helper
#include "android/keyboard_helper.h"



// Forward declarations for Android NDK types
struct android_app;
struct android_poll_source {
    int32_t id;
    android_app* app;
    void (*process)(android_app* app, android_poll_source* source);
};

// Forward declaration for android_app structure
struct android_app {
    void* userData;
    void (*onAppCmd)(android_app* app, int32_t cmd);
    int32_t (*onInputEvent)(android_app* app, AInputEvent* event);
    struct ANativeActivity* activity;
    struct AConfiguration* config;
    ANativeWindow* window;  // Added window member
    // ... other fields not needed for this example
};

// ImGui Android implementation
extern bool ImGui_ImplAndroid_Init(ANativeWindow* window);
extern void ImGui_ImplAndroid_Shutdown();
extern void ImGui_ImplAndroid_NewFrame();
extern void ImGui_ImplAndroid_RenderDrawData(ImDrawData* draw_data);
extern bool ImGui_ImplAndroid_HandleInputEvent(const AInputEvent* input_event);

// Forward declaration of helper function
static AInputEvent* createTouchEvent(int32_t action, int32_t pointer_id, float x, float y);

// Helper function to get the Android app instance
static struct android_app* ImGui_ImplAndroid_GetApp() {
    // Implementation depends on how you store the android_app pointer
    // This is just a placeholder
    return (struct android_app*)Application::getInstance();  // Modified to avoid getPlatform()
}


PlatformAndroid::PlatformAndroid(const std::string& title, LogWidget* logWidget)
    : PlatformBase(title, logWidget), m_androidApp(nullptr), m_keyboardVisible(false) {
	 
}

PlatformAndroid::~PlatformAndroid() {
    platformShutdown();
}

void PlatformAndroid::setAndroidApp(void* app) {
    m_androidApp = app;
    
}

void* PlatformAndroid::getAndroidApp() {
    return m_androidApp;
}

bool PlatformAndroid::initWithWindow(ANativeWindow* window) {
    if (!window) {
        LOG_ERROR("Null window passed to initWithWindow");
        return false;
    }
    
    // Store the window pointer directly
    m_window = window;
    
    // Always create a new context for a new window
    m_imguiContext = ImGui::CreateContext();
    if (!m_imguiContext) {
        LOG_ERROR("Failed to create ImGui context");
        return false;
    }
    ImGui::SetCurrentContext(m_imguiContext);
    
    // Configure ImGui style
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    
    // Get window dimensions to handle orientation
    if (m_window) {
        int32_t width = ANativeWindow_getWidth(m_window);
        int32_t height = ANativeWindow_getHeight(m_window);
        io.DisplaySize = ImVec2((float)width, (float)height);
    }
    
    // Initialize ImGui for Android with the direct window pointer
    bool success = ImGui_ImplAndroid_Init(m_window);
    
    return success;
}

bool PlatformAndroid::platformInit() {
    struct android_app* app = (struct android_app*)m_androidApp;
    if (!app) {
        LOG_ERROR("No Android app in platformInit");
        return false;
    }
    
    
    
    // If we already have a stored window pointer, use that
    if (m_window) {
        
        return initWithWindow(m_window);
    }
    
    // Otherwise try to get the window from the app
    if (!app->window) {
        LOG_ERROR("No window available in platformInit");
        return false;
    }
    
    // Store the window pointer and initialize
    return initWithWindow(app->window);
}

void PlatformAndroid::platformNewFrame() {
    // Call ImGui_ImplAndroid_NewFrame but don't call ImGui::NewFrame() here
    // ImGui::NewFrame() is called in Application::renderFrame()
    ImGui_ImplAndroid_NewFrame();
    
    // Check ImGui's WantTextInput flag to show/hide keyboard
    // This is done here so it's checked every frame
    ImGuiIO& io = ImGui::GetIO();
    bool wantsTextInput = io.WantTextInput;
    
    // Show or hide keyboard based on ImGui's WantTextInput flag
    if (wantsTextInput && !m_keyboardVisible) {
        // ImGui wants text input - show keyboard
        showKeyboard();
        m_keyboardVisible = true;
        
    } else if (!wantsTextInput && m_keyboardVisible) {
        // ImGui no longer wants text input - hide keyboard
        hideKeyboard();
        m_keyboardVisible = false;
        
    }
    
    // Note: Don't call ImGui::NewFrame() here, it's called in Application::renderFrame()
}

void PlatformAndroid::platformRender() {
    // Render ImGui - don't call ImGui::NewFrame() here!
    // Just render the data that was prepared in Application::renderFrame()
    ImGui_ImplAndroid_RenderDrawData(ImGui::GetDrawData());
}

bool PlatformAndroid::platformHandleEvents() {
    struct android_app* app = (struct android_app*)m_androidApp;
    if (!app) return false;
    
    // Process Android events
    int events;
    android_poll_source* source;
    
    // Poll for events
    if (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
        if (source != nullptr) {
            source->process(app, source);
        }
    }
    
    return true;
}

void PlatformAndroid::platformShutdown() {
    // First shut down ImGui Android implementation
    ImGui_ImplAndroid_Shutdown();
    
    // Only destroy the ImGui context if it exists
    if (m_imguiContext) {
        ImGui::SetCurrentContext(m_imguiContext); // Set the context as current before destroying
        ImGui::DestroyContext(m_imguiContext);
        m_imguiContext = nullptr;
    }
    
    // Clear the window pointer
    m_window = nullptr;
    
    // Reset the Android app pointer
    m_androidApp = nullptr;
}

// Touch handling functions
bool handleTouchDown(int pointer_id, float x, float y) {
    AInputEvent* event = createTouchEvent(AMOTION_EVENT_ACTION_DOWN, pointer_id, x, y);
    bool result = ImGui_ImplAndroid_HandleInputEvent(event);
    // Free event if needed
    return result;
}

bool handleTouchMove(int pointer_id, float x, float y) {
    AInputEvent* event = createTouchEvent(AMOTION_EVENT_ACTION_MOVE, pointer_id, x, y);
    bool result = ImGui_ImplAndroid_HandleInputEvent(event);
    // Free event if needed
    return result;
}

bool handleTouchUp(int pointer_id, float x, float y) {
    AInputEvent* event = createTouchEvent(AMOTION_EVENT_ACTION_UP, pointer_id, x, y);
    bool result = ImGui_ImplAndroid_HandleInputEvent(event);
    // Free event if needed
    return result;
}

bool handleTouchCancel(int pointer_id, float x, float y) {
    AInputEvent* event = createTouchEvent(AMOTION_EVENT_ACTION_CANCEL, pointer_id, x, y);
    bool result = ImGui_ImplAndroid_HandleInputEvent(event);
    // Free event if needed
    return result;
}

// Helper function to create touch events
static AInputEvent* createTouchEvent(int32_t action, int32_t pointer_id, float x, float y) {
    // This is a placeholder - in a real implementation, you would use the Android NDK
    // to create proper input events. This is complex and requires more code than shown here.
    // For now, we'll return nullptr to indicate this needs proper implementation
    return nullptr;
}

void PlatformAndroid::recreateSwapChain() {
    ImGui_ImplAndroid_Shutdown();
    if (m_androidApp) {
        struct android_app* app = (struct android_app*)m_androidApp;
        if (app->window) {
            initWithWindow(app->window);
        }
    }
}
