#include <android_native_app_glue.h>
#include <unistd.h> // For chdir
#include <sys/stat.h> // For mkdir
#include "../../include/platform/platform_android.h"
#include <EGL/egl.h>
#include "../../include/application.h"
#include "../../include/platform/logger.h"
#include "../../include/platform/android/platform_android_logger.h"
#include "../../include/widget/log_widget.h"
#include "../../include/platform/scaling_manager.h"
#include "../../include/platform/state_manager.h" // Include StateManager
#include <string> // For std::string

// Forward declaration for ImGui Android functions
extern bool ImGui_ImplAndroid_HandleInputEvent(const AInputEvent* event);
extern void ImGui_ImplAndroid_SetAssetManager(AAssetManager* assetManager);

// Global application instance
static PlatformAndroid* g_app = nullptr;
bool g_initialized = false;
static ANativeWindow* g_savedWindow = nullptr;

// Global LogWidget instance
    static LogWidget g_logWidget;

// Process Android command events
static void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (app->window != nullptr) {
                g_savedWindow = app->window;
                if (g_app) {
                    g_app->setAndroidApp(app);
                    ImGui_ImplAndroid_SetAssetManager(app->activity->assetManager);
                    if (g_app->initWithWindow(g_savedWindow)) {
                        g_initialized = true;
                    } else {
                        LOG_ERROR("Platform initialization failed");
                    }
                }
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            if (g_app && g_initialized) {
                g_app->platformShutdown();
                g_initialized = false;
            }
            g_savedWindow = nullptr;
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_CONFIG_CHANGED:
            // This is just a notification. The OS will follow up with
            // TERM_WINDOW and INIT_WINDOW to actually recreate the surface.
            if (g_app) {
                ScalingManager::getInstance().setConfiguration(app->config);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // Save state when app loses focus
            StateManager::getInstance().saveStateAsync();
            break;
        default:
            break;
    }
}

// Process Android input events
static int32_t handle_input(android_app* app, AInputEvent* event) {
    // Forward to ImGui
    return ImGui_ImplAndroid_HandleInputEvent(event);
}

// Main entry point for Android applications using native_app_glue
void android_main(struct android_app* app) {
    // Global LogWidget instance
    static LogWidget g_logWidget;

    // Initialize the logger
    g_logger = LoggerFactory::createLogger().release(); // Initialize global logger
    AndroidPlatformLogger* android_logger = dynamic_cast<AndroidPlatformLogger*>(g_logger);
    if (android_logger) {
        android_logger->set_log_widget(&g_logWidget);
    }

    // Get the package name from ANativeActivity and set it in the LoggerFactory
    JNIEnv* env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);
    jclass activityClass = env->GetObjectClass(app->activity->clazz);
    jmethodID getPackageNameMethod = env->GetMethodID(activityClass, "getPackageName", "()Ljava/lang/String;");
    jstring jPackageName = (jstring)env->CallObjectMethod(app->activity->clazz, getPackageNameMethod);
    const char* packageNameCStr = env->GetStringUTFChars(jPackageName, nullptr);
    if (packageNameCStr != nullptr) {
        LoggerFactory::setPackageName(packageNameCStr);
        env->ReleaseStringUTFChars(jPackageName, packageNameCStr);
    }
    app->activity->vm->DetachCurrentThread();

    // Make sure glue isn't stripped
    app_dummy();

    // Debugging: Log the initial directory change message with hex values
    const char* initial_path = app->activity->internalDataPath;
    std::string debug_str = "";
    for (int i = 0; i < std::min((int)strlen(initial_path), 10); ++i) {
        debug_str += std::to_string((int)initial_path[strlen(initial_path) - 10 + i]) + " ";
    }
    LOG_INFO("LogWidget: Initial path last 10 chars (hex): %s", debug_str.c_str());

    // Change the current directory to the app's internal data directory
    if (app->activity->internalDataPath != nullptr) {
        const char* path = app->activity->internalDataPath;
        LOG_INFO("Attempting to change directory to: %s", path);
        
        // Create the directory if it doesn't exist
        struct stat st = {0};
        if (stat(path, &st) == -1) {
            mkdir(path, 0700);
        }
        if (chdir(path) == 0) {
            LOG_INFO("Successfully changed directory to: %s", path);
            
            StateManager::getInstance().setInternalDataPath(path); // Set path for StateManager
        } else {
            LOG_ERROR("Failed to change directory to: %s", path);
        }
    } else {
        LOG_ERROR("Internal data path is null, cannot change directory.");
    }
    
    // Set callbacks
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    
    // Create application instance
    g_app = new PlatformAndroid("ImGui Hello World", &g_logWidget);

    // Load state at startup
        StateManager::getInstance().loadStateAsync();
    
    // Reset the scaling manager to force scaling application
    ScalingManager& scalingManager = ScalingManager::getInstance();
    scalingManager.reset();
    
    // Set the Android configuration in the scaling manager
    if (app && app->config) {
        scalingManager.setConfiguration(app->config);
        LOG_INFO("Android configuration set in ScalingManager");
    } else {
        LOG_ERROR("No Android configuration available for ScalingManager");
    }
    
    // Set a scale adjustment factor if needed (1.0 = use the exact density-based scale)
    // You can adjust this value based on your device preferences
    scalingManager.setScaleAdjustment(1.0f);  // Adjusted for better visibility
    LOG_INFO("Scale adjustment set to 1.0 for better visibility");
    
    
    // Force initialization immediately
    if (app->window != nullptr) {
        g_savedWindow = app->window;
        g_app->setAndroidApp(app);
        
        // Initialize with proper scaling
        bool success = g_app->initWithWindow(g_savedWindow);
        if (success) {
            LOG_INFO("Platform initialized successfully at startup");
            
            g_initialized = true;
            
            // Force a small delay to ensure initialization completes
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 100000000; // 100ms
            nanosleep(&ts, NULL);
        } else {
            LOG_ERROR("Failed to initialize at startup");
        }
    }
    
    
    
    // Main loop
    while (1) {
        // Read all pending events
        int events;
        android_poll_source* source;
        
        // Process events - block until we get events
        while ((ALooper_pollAll(g_initialized ? 0 : -1, nullptr, &events, (void**)&source)) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }
            
            // Check if we are exiting
            if (app->destroyRequested != 0) {
                if (g_app) {
                    delete g_app;
                    g_app = nullptr;
                }
                if (g_logger) { // Delete global logger
                    delete g_logger;
                    g_logger = nullptr;
                }
                StateManager::getInstance().saveStateAsync(); // Save state on exit
                g_logWidget.Clear(); // Clear log widget to prevent memory leak
                return;
            }
        }
        
        // If initialized, run the application frame
        if (g_initialized) {
            // Process tasks queued for the main thread
            Application::getInstance()->processMainThreadTasks();

            // Run a single frame of the application
            Application::getInstance()->renderFrame();
            
            // Sleep a bit to avoid busy waiting
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 16 * 1000000; // 16ms ~= 60fps
            nanosleep(&ts, NULL);
        }
    }
}

void PlatformAndroid::platformRender()
{
    // Get the EGL surface dimensions directly to ensure correctness after rotation
    EGLint fb_width, fb_height;
    eglQuerySurface(g_EglDisplay, g_EglSurface, EGL_WIDTH, &fb_width);
    eglQuerySurface(g_EglDisplay, g_EglSurface, EGL_HEIGHT, &fb_height);
    m_fbWidth = fb_width;
    m_fbHeight = fb_height;

    ImGui_ImplAndroid_RenderDrawData(ImGui::GetDrawData());
}

int PlatformAndroid::getFramebufferWidth() const {
    return m_fbWidth;
}

int PlatformAndroid::getFramebufferHeight() const {
    return m_fbHeight;
}
