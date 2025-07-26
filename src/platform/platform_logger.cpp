#include "../../../include/platform/platform_logger.h"
#include "../../../include/logger.h"
#include <iostream>
#include <cstdarg>

// Standard output/error logger (default for non-Android platforms)
class StdPlatformLogger : public IPlatformLogger {
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

// Factory implementation
std::unique_ptr<IPlatformLogger> PlatformLoggerFactory::createPlatformLogger() {
    return std::make_unique<StdPlatformLogger>();
}