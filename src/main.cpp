#include <iostream>
#include "../include/platform/logger.h"
#include "../include/log_widget.h" // Include log_widget.h
#include <unistd.h> // For chdir

#if defined(__ANDROID__)
    #include "../include/platform/platform_android.h"
    #include "../include/platform/android/platform_android_logger.h"
    typedef PlatformAndroid PlatformType;
#elif defined(__EMSCRIPTEN__)
    #include "../include/platform/platform_wasm.h"
    typedef PlatformWasm PlatformType;
#else
    // Default to GLFW for desktop platforms (Windows, Linux, macOS)
    #include "../include/platform/platform_glfw.h"
    typedef PlatformGLFW PlatformType;
#endif

int main(int argc, char** argv)
{
    // Initialize the logger
    g_logger = LoggerFactory::createLogger().release();

#if !defined(__ANDROID__)
    static LogWidget main_log_widget;
    // Change current working directory to the build output directory
    // This assumes the executable is run from the build/linux/bin directory
    // and assets are copied to build/linux/assets.
    if (chdir("../") != 0) {
        LOG_ERROR("Failed to change directory to ../");
        return 1;
    }
#else
    static LogWidget main_log_widget;
    AndroidPlatformLogger* android_logger = dynamic_cast<AndroidPlatformLogger*>(g_logger);
    if (android_logger) {
        android_logger->set_log_widget(&main_log_widget);
    }
#endif

    try {
        // Create platform-specific application instance
#if !defined(__ANDROID__)
        PlatformType app("ImGui Hello World", 1280, 720); // Default width and height
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
