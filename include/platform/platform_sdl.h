#pragma once

#include "platform_base.h"
#include <string>
#include <SDL.h>

class PlatformSDL : public PlatformBase {
public:
    PlatformSDL(const std::string& title, int width, int height);
    virtual ~PlatformSDL();

    void run();
    void set_window_title(const std::string& title);

private:
    SDL_Window* m_window;
    SDL_GLContext m_gl_context;
    bool m_done;
    int m_width;
    int m_height;

    // Pure virtual methods from Application (via PlatformBase)
    bool platformInit() override;
    void platformShutdown() override;
    void platformNewFrame() override;
    void platformRender() override;
    bool platformHandleEvents() override;
    int getFramebufferWidth() const override;
    int getFramebufferHeight() const override;
};
