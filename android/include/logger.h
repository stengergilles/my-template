#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>

// Enum for log levels
enum class LogLevel {
    Info,
    Warning,
    Error
};

// Abstract base class for the logger
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(LogLevel level, const char* fmt, ...) = 0;
};

// Factory to create the appropriate logger for the platform
class LoggerFactory {
public:
    static std::unique_ptr<ILogger> createLogger();
};

// Global logger instance
extern std::unique_ptr<ILogger> g_logger;

// Helper macros for easy logging
#define LOG_INFO(fmt, ...) g_logger->log(LogLevel::Info, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) g_logger->log(LogLevel::Warning, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) g_logger->log(LogLevel::Error, fmt, ##__VA_ARGS__)

#endif // LOGGER_H
