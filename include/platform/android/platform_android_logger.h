#ifndef PLATFORM_ANDROID_LOGGER_H
#define PLATFORM_ANDROID_LOGGER_H

#include "../../../include/platform/platform_logger.h"
#include "../../widget/log_widget.h"

class AndroidPlatformLogger : public IPlatformLogger {
public:
    AndroidPlatformLogger();
    void set_log_widget(LogWidget* widget);
    void log(LogLevel level, const char* fmt, ...) override;

private:
    LogWidget* m_log_widget;
};

#endif // PLATFORM_ANDROID_LOGGER_H
