#include "platform/platform_logger.h"
#include <android/log.h>
#include <string>

extern std::string g_PackageName;

namespace platform_logger {
    void log_info(const std::string& tag, const std::string& message) {
        __android_log_print(ANDROID_LOG_INFO, g_PackageName.empty() ? tag.c_str() : g_PackageName.c_str(), "%s", message.c_str());
    }

    void log_error(const std::string& tag, const std::string& message) {
        __android_log_print(ANDROID_LOG_ERROR, g_PackageName.empty() ? tag.c_str() : g_PackageName.c_str(), "%s", message.c_str());
    }
}
