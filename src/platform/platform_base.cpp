#include "../include/platform/platform_base.h"

PlatformBase::PlatformBase(const std::string& appName, LogWidget* logWidget)
    : Application(appName, logWidget)
{
}

PlatformBase::~PlatformBase()
{
    // Base platform cleanup
}
