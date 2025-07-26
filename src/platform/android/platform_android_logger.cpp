#include "../../../../include/platform/android/platform_android_logger.h"
#include "../../../../include/logger.h"
#include <android/log.h>
#include <cstdarg>

AndroidPlatformLogger::AndroidPlatformLogger() : m_log_widget(nullptr) {}

void AndroidPlatformLogger::set_log_widget(LogWidget* widget) {
    m_log_widget = widget;
}

void AndroidPlatformLogger::log(LogLevel level, const char* fmt, ...) {
    int android_level;
    switch (level) {
        case LogLevel::Info:
            android_level = ANDROID_LOG_INFO;
            break;
        case LogLevel::Warning:
            android_level = ANDROID_LOG_WARN;
            break;
        case LogLevel::Error:
            android_level = ANDROID_LOG_ERROR;
            break;
    }

    va_list args_android;
    va_list args_widget;

    va_start(args_android, fmt);
    va_copy(args_widget, args_android);

    __android_log_vprint(android_level, LoggerFactory::getPackageName().c_str(), fmt, args_android);
    
    if (m_log_widget) {
        m_log_widget->AddLogV(fmt, args_widget);
    }

    va_end(args_android);
    va_end(args_widget);
}