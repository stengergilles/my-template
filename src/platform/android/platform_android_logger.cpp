#include "platform/platform_logger.h"
#include <android/log.h>

namespace platform_logger {
    void log_info(const std::string& tag, const std::string& message) {
        __android_log_print(ANDROID_LOG_INFO, tag.c_str(), "%s", message.c_str());
    }

    void log_error(const std::string& tag, const std::string& message) {
        __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), "%s", message.c_str());
    }
}
