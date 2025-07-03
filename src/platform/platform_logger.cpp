#include "platform/platform_logger.h"
#include <iostream>

namespace platform_logger {
    void log_info(const std::string& tag, const std::string& message) {
        std::cout << "INFO [" << tag << "]: " << message << std::endl;
    }

    void log_error(const std::string& tag, const std::string& message) {
        std::cerr << "ERROR [" << tag << "]: " << message << std::endl;
    }
}
