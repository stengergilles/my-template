#include "platform/platform_logger.h"
#include <android/log.h>
#include <string>
#include <cstdarg> // For va_list
#include "../../include/logger.h" // For ILogger and LogLevel

extern std::string g_PackageName;

namespace platform_logger {

class AndroidLogger : public ILogger {
public:
    void log(LogLevel level, const char* fmt, ...) override {
        int android_log_level;
        switch (level) {
            case LogLevel::Info:
                android_log_level = ANDROID_LOG_INFO;
                break;
            case LogLevel::Warning:
                android_log_level = ANDROID_LOG_WARN;
                break;
            case LogLevel::Error:
                android_log_level = ANDROID_LOG_ERROR;
                break;
            default:
                android_log_level = ANDROID_LOG_DEFAULT;
                break;
        }

        va_list args;
        va_start(args, fmt);
        __android_log_vprint(android_log_level, g_PackageName.empty() ? "ImGuiApp" : g_PackageName.c_str(), fmt, args);
        va_end(args);
    }
};

} // namespace platform_logger

std::unique_ptr<ILogger> LoggerFactory::createLogger() {
    return std::make_unique<platform_logger::AndroidLogger>();
}

#if defined(__ANDROID__)
void LoggerFactory::set_android_logger_widget(LogWidget* widget) {
    // This function is called to set the LogWidget for Android.
    // The AndroidLogger class above directly uses __android_log_print,
    // so we don't need to pass the widget to it.
    // However, if you want to also display logs in the ImGui LogWidget,
    // you would need to modify the AndroidLogger to also call widget->AddLog.
    // For now, we'll leave this empty as the primary goal is to unify
    // the logging interface.
}
#endif
