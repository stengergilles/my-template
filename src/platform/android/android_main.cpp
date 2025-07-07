#include <android/log.h>
#include <android_native_app_glue.h>
#include <unistd.h> // For chdir
#include <sys/stat.h> // For mkdir
#include "../../include/platform/platform_android.h"
#include "../../include/application.h"
#include "../../include/logger.h"
#include "../../include/scaling_manager.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ImGuiApp", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "ImGuiApp", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "ImGuiApp", __VA_ARGS__))

// Forward declaration for ImGui Android functions
extern bool ImGui_ImplAndroid_HandleInputEvent(const AInputEvent* event);

// Global application instance
static PlatformAndroid* g_app = nullptr;
bool g_initialized = false;
static ANativeWindow* g_savedWindow = nullptr;

// Process Android command events
static void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != nullptr) {
                g_savedWindow = app->window;
                if (g_app && !g_initialized) {
                    g_app->setAndroidApp(app);
                    if (g_app->initWithWindow(g_savedWindow)) {
                        g_initialized = true;
                    } else {
                        LOGE("Platform initialization failed");
                    }
                }
            }
            break;
        case APP_CMD_TERM_WINDOW:
            if (g_app && g_initialized) {
                g_app->platformShutdown();
                g_initialized = false;
            }
            g_savedWindow = nullptr;
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_CONFIG_CHANGED:
            // Configuration changed (e.g., orientation)
            // The window will be re-initialized by the system, so we don't need to do anything here.
            // The APP_CMD_TERM_WINDOW and APP_CMD_INIT_WINDOW events will handle the shutdown and re-initialization.
            break;
        case APP_CMD_LOST_FOCUS:
            break;
        default:
            break;
    }
}

// Process Android input events
static int32_t handle_input(android_app* app, AInputEvent* event) {
    // Forward to ImGui
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        // Handle touch events
        bool handled = ImGui_ImplAndroid_HandleInputEvent(event);
        return handled ? 1 : 0;
    }
    return 0; // Event was not handled
}

// Main entry point for Android applications using native_app_glue
void android_main(struct android_app* app) {
    // Initialize the logger
    g_logger = LoggerFactory::createLogger();

    // Make sure glue isn't stripped
    app_dummy();

    // Change the current directory to the app's internal data directory
    if (app->activity->internalDataPath != nullptr) {
        const char* path = app->activity->internalDataPath;
        LOGI("Attempting to change directory to: %s", path);
        // Create the directory if it doesn't exist
        struct stat st = {0};
        if (stat(path, &st) == -1) {
            mkdir(path, 0700);
        }
        if (chdir(path) == 0) {
            LOGI("Successfully changed directory to: %s", path);
        } else {
            LOGE("Failed to change directory to: %s", path);
        }
    } else {
        LOGE("Internal data path is null, cannot change directory.");
    }
    
    // Set callbacks
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    
    // Create application instance
    g_app = new PlatformAndroid("ImGui Hello World");
    
    // Reset the scaling manager to force scaling application
    ScalingManager& scalingManager = ScalingManager::getInstance();
    scalingManager.reset();
    
    // Set the Android configuration in the scaling manager
    if (app && app->config) {
        scalingManager.setConfiguration(app->config);
        LOGI("Android configuration set in ScalingManager");
    } else {
        LOGE("No Android configuration available for ScalingManager");
    }
    
    // Set a scale adjustment factor if needed (1.0 = use the exact density-based scale)
    // You can adjust this value based on your device preferences
    scalingManager.setScaleAdjustment(1.5f);  // Increased to 1.5 for better visibility
    LOGI("Scale adjustment set to 1.5 for better visibility");
    
    // Force initialization immediately
    if (app->window != nullptr) {
        g_savedWindow = app->window;
        g_app->setAndroidApp(app);
        
        // Initialize with proper scaling
        bool success = g_app->initWithWindow(g_savedWindow);
        if (success) {
            LOGI("Platform initialized successfully at startup");
            g_initialized = true;
            
            // Force a small delay to ensure initialization completes
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 100000000; // 100ms
            nanosleep(&ts, NULL);
        } else {
            LOGE("Failed to initialize at startup");
        }
    }
    
    LOGI("Starting application main loop");
    
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
                LOGI("Exiting application");
                if (g_app) {
                    delete g_app;
                    g_app = nullptr;
                }
                return;
            }
        }
        
        // If initialized, run the application frame
        if (g_initialized) {
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