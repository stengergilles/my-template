#ifndef PLATFORM_LOGGER_H
#define PLATFORM_LOGGER_H

#include <memory>
#include <string>
#include "../../include/platform/logger.h" // For LogLevel and ILogger

class LogWidget;

// Abstract base class for platform-specific logger implementations
class IPlatformLogger : public ILogger {
public:
    virtual ~IPlatformLogger() = default;
    // Inherits log method from ILogger
};

// Factory to create the appropriate platform-specific logger
class PlatformLoggerFactory {
public:
    static std::unique_ptr<IPlatformLogger> createPlatformLogger();
};

#endif // PLATFORM_LOGGER_H
