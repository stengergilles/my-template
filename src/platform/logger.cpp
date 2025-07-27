#include "../../include/platform/logger.h"
#include "../../include/log_widget.h"
#include "../../include/platform/platform_logger.h"
#if defined(__ANDROID__)
#include "../../include/platform/android/platform_android_logger.h"
#endif
#include <iostream>
#include <cstdarg>

// Global logger instance
ILogger* g_logger = nullptr;

// Initialize static member
std::string LoggerFactory::s_packageName = "";

// Factory implementation
std::unique_ptr<ILogger> LoggerFactory::createLogger() {
#if defined(__ANDROID__)
    return std::make_unique<AndroidPlatformLogger>();
#else
    return PlatformLoggerFactory::createPlatformLogger();
#endif
}

void LoggerFactory::setPackageName(const std::string& packageName) {
    s_packageName = packageName;
}

const std::string& LoggerFactory::getPackageName() {
    return s_packageName;
}


