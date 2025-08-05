#include <iostream>
#include "../../include/platform/logger.h"
#include "../../include/widget/log_widget.h" // Include log_widget.h
#include <unistd.h> // For chdir, readlink
#include <sys/stat.h> // For mkdir
#include <string>
#include <algorithm>
#include <limits.h> // For PATH_MAX
#include <libgen.h> // For dirname
#include <filesystem> // For std::filesystem
#include "../../include/platform/state_manager.h" // For StateManager
#include "../../include/platform/font_manager.h" // For FontManager

// Helper function to convert package name to camel case
std::string toCamelCase(const std::string& s) {
    std::string result = "";
    bool capitalizeNext = false;
    for (char c : s) {
        if (c == '.') {
            capitalizeNext = true;
        } else {
            if (capitalizeNext) {
                result += toupper(c);
                capitalizeNext = false;
            } else {
                result += c;
            }
        }
    }
    return result;
}

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
    // Get executable path to determine asset location
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
    } else {
        LOG_ERROR("Failed to get executable path.");
        return 1;
    }

    std::filesystem::path executableDir = std::filesystem::path(exePath).parent_path();
    std::filesystem::path sourceAssetsDir = executableDir / "assets";

    // Change current working directory to the build output directory
    // This assumes the executable is run from the build/linux/bin directory
    // and assets are copied to build/linux/assets.
    if (chdir("../") != 0) {
        LOG_ERROR("Failed to change directory to ../");
        return 1;
    }

    // Construct app home directory path
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        LOG_ERROR("HOME environment variable not set.");
        return 1;
    }

    std::string packageName = LINUX_APP_PACKAGE_NAME; // From CMake definition
    std::string camelCasePackageName = toCamelCase(packageName);
    std::string appHomeDir = std::string(homeDir) + "/." + camelCasePackageName;

    // Create app home directory if it doesn't exist
    struct stat st = {0};
    if (stat(appHomeDir.c_str(), &st) == -1) {
        LOG_INFO("Creating application home directory: %s", appHomeDir.c_str());
        if (mkdir(appHomeDir.c_str(), 0700) == -1) {
            LOG_ERROR("Failed to create application home directory: %s", appHomeDir.c_str());
            return 1;
        }
    }

    // Set StateManager's internal data path
    StateManager::getInstance().setInternalDataPath(appHomeDir);

    // Copy fonts from sourceAssetsDir to app home directory
    for (const auto& entry : std::filesystem::directory_iterator(sourceAssetsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".ttf") {
            std::filesystem::path destPath = appHomeDir / entry.path().filename();
            if (!std::filesystem::exists(destPath)) {
                std::filesystem::copy(entry.path(), destPath);
                LOG_INFO("Copied font: %s to %s", entry.path().filename().c_str(), appHomeDir.c_str());
            }
        }
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
        PlatformType app("ImGui Hello World", 720, 1280); // Default width and height
        StateManager::getInstance().loadStateAsync();
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