#include "../include/logger.h"
#include "../include/log_widget.h"
#include <iostream>
#include <cstdarg>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

// Global logger instance
ILogger* g_logger = nullptr;

// Standard output/error logger
class StdLogger : public ILogger {
public:
    void log(LogLevel level, const char* fmt, ...) override {
        va_list args;
        va_start(args, fmt);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        switch (level) {
            case LogLevel::Info:
                std::cout << "[INFO] " << buffer << std::endl;
                break;
            case LogLevel::Warning:
                std::cerr << "[WARNING] " << buffer << std::endl;
                break;
            case LogLevel::Error:
                std::cerr << "[ERROR] " << buffer << std::endl;
                break;
        }
    }
};

#if defined(__ANDROID__)
// Android logcat logger
class AndroidLogger : public ILogger {
public:
    AndroidLogger() : m_log_widget(nullptr) {}

    void set_log_widget(LogWidget* widget) {
        m_log_widget = widget;
    }

    void log(LogLevel level, const char* fmt, ...) override {
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

        __android_log_vprint(android_level, "ImGuiApp", fmt, args_android);
        
        if (m_log_widget) {
            m_log_widget->AddLogV(fmt, args_widget);
        }

        va_end(args_android);
        va_end(args_widget);
    }

private:
    LogWidget* m_log_widget;
};
#endif

// Factory implementation
std::unique_ptr<ILogger> LoggerFactory::createLogger() {
#if defined(__ANDROID__)
    return std::make_unique<AndroidLogger>();
#else
    return std::make_unique<StdLogger>();
#endif
}

#if defined(__ANDROID__)
void LoggerFactory::set_android_logger_widget(LogWidget* widget) {
#if defined(__ANDROID__)
    if (g_logger) {
        AndroidLogger* android_logger = dynamic_cast<AndroidLogger*>(g_logger);
        if (android_logger) {
            android_logger->set_log_widget(widget);
        }
    }
#endif
}
#endif
