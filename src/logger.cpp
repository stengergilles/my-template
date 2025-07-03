#include "../include/logger.h"
#include <iostream>
#include <cstdarg>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

// Global logger instance
std::unique_ptr<ILogger> g_logger;

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

        va_list args;
        va_start(args, fmt);
        __android_log_vprint(android_level, "ImGuiApp", fmt, args);
        va_end(args);
    }
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
