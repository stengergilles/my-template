#pragma once

#include <string>

namespace platform_logger {
    void log_info(const std::string& tag, const std::string& message);
    void log_error(const std::string& tag, const std::string& message);
}
