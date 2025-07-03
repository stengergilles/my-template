#include <iostream>
#include "../include/logger.h"

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

int main(int argc, char** argv)
{
    // Initialize the logger
    g_logger = LoggerFactory::createLogger();

    try {
        // Create platform-specific application instance
        PlatformType app("ImGui Hello World");
        
        // Run the application
        app.run();
        
        return 0;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Error: %s", e.what());
        return 1;
    }
}