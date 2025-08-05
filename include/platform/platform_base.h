#pragma once

#include "../application.h"

// Base class for platform-specific implementations
class PlatformBase : public Application {
public:
    PlatformBase(const std::string& appName = "ImGui Hello World", LogWidget* logWidget = nullptr);
    virtual ~PlatformBase();
    
    virtual void initializeImGui() {};
    // Add a method to get the Android app pointer
    virtual void* getAndroidApp() { return nullptr; }

protected:
    // Common platform functionality can be implemented here
};
