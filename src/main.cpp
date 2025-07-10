#include <iostream>
#include "../include/logger.h"
#include "../include/log_widget.h" // Include log_widget.h

#if defined(__ANDROID__)
    #include "../include/platform/platform_android.h"
    typedef PlatformAndroid PlatformType;
#elif defined(__EMSCRIPTEN__)
    #include "../include/platform/platform_wasm.h"
    typedef PlatformWasm PlatformType;
#else
    // Default to GLFW for desktop platforms (Windows, Linux, macOS)
    #include "../include/platform/platform_glfw.h"
    typedef PlatformGLFW PlatformType;
#endif

#if !defined(__ANDROID__)
static LogWidget main_log_widget;
#endif

int main(int argc, char** argv)
{
    // Initialize the logger
    g_logger = LoggerFactory::createLogger();

#if !defined(__ANDROID__)
    LoggerFactory::set_android_logger_widget(&main_log_widget);
#endif

    try {
        // Create platform-specific application instance
#if !defined(__ANDROID__)
        PlatformType app("ImGui Hello World", &main_log_widget);
#else
        PlatformType app("ImGui Hello World", nullptr); // Pass nullptr for Android
#endif
        
        // Run the application
        app.run();
        
        return 0;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Error: %s", e.what());
        return 1;
    }
}