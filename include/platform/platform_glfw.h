#pragma once

#include "platform_base.h"

// Forward declarations
struct GLFWwindow;

// GLFW implementation (works on Windows, Linux, macOS)
class PlatformGLFW : public PlatformBase {
public:
    PlatformGLFW(const std::string& appName,int width = 1280, int height = 720);
    virtual ~PlatformGLFW();

    // Main application loop
    void run();

protected:
    virtual bool platformInit() override;
    virtual void platformShutdown() override;
    virtual void platformNewFrame() override;
    virtual void platformRender() override;
    virtual bool platformHandleEvents() override;
    virtual int getFramebufferWidth() const override;
    virtual int getFramebufferHeight() const override;

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
};
