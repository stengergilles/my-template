#include <iostream>
#include "../../include/platform/logger.h"
#include "../../include/widget/log_widget.h" // Include log_widget.h
#include <unistd.h> // For chdir

#if defined(LINUX)
    #include "../../include/platform/platform_sdl.h"
    typedef PlatformSDL PlatformType;
#elif (defined(__ANDROID__))
    #include "../../include/platform/platform_android.h"
    #include "../../include/platform/android/platform_android_logger.h"
    typedef PlatformAndroid PlatformType;
#elif defined(__EMSCRIPTEN__)
    #include "../../include/platform/platform_wasm.h"
    typedef PlatformWasm PlatformType;
#else
    #include "../../include/platform/platform_sdl.h"
    typedef PlatformSDL PlatformType;
#endif

int main(int argc, char** argv)
{
    // Initialize the logger
    g_logger = LoggerFactory::createLogger().release();

#if defined(LINUX)
    static LogWidget main_log_widget;
    // Change current working directory to the build output directory
    // This assumes the executable is run from the build/linux/bin directory
    // and assets are copied to build/linux/assets.
    if (chdir("../") != 0) {
        LOG_ERROR("Failed to change directory to ../");
        return 1;
    }
#elif (defined(__ANDROID__))
    static LogWidget main_log_widget;
    AndroidPlatformLogger* android_logger = dynamic_cast<AndroidPlatformLogger*>(g_logger);
    if (android_logger) {
        android_logger->set_log_widget(&main_log_widget);
    }
#else
    static LogWidget main_log_widget;
    // Change current working directory to the build output directory
    // This assumes the executable is run from the build/linux/bin directory
    // and assets are copied to build/linux/assets.
    if (chdir("../") != 0) {
        LOG_ERROR("Failed to change directory to ../");
        return 1;
    }
#endif

    try {
        // Create platform-specific application instance
#if defined(LINUX)
        PlatformType app("ImGui Hello World", 1280, 720); // Default width and height
#elif (defined(__ANDROID__))
        PlatformType app("ImGui Hello World", nullptr); // Pass nullptr for Android
#else
        PlatformType app("ImGui Hello World", 1280, 720); // Default width and height
#endif
        
#if defined(LINUX)
        // Run the application
        app.run();
#elif (defined(__ANDROID__))
        // For android, app.run() is not called from main.
#else
        // Run the application
        app.run();
#endif
        
        return 0;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Error: %s", e.what());
        return 1;
    }
}