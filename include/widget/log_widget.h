#pragma once

#include "imgui.h"
#include <vector>
#include <mutex>

class LogWidget {
public:
    LogWidget(size_t max_size = 2000, size_t max_lines = 500);
    ~LogWidget();

    void AddLog(const char* fmt, ...);
    void AddLogV(const char* fmt, va_list args);
    void Draw(const char* title, bool* p_open = NULL);
    void Clear();

private:
    size_t max_size;
    size_t max_lines;
    char*                     Buf;          // Circular buffer
    std::vector<int>          LineOffsets;  // Index to first char of each line
    int                       BufOffset;    // Current write position in Buf
    bool                      ScrollToBottom;
    std::mutex                LogMutex;
    static LogWidget* s_instance;

public:
    static LogWidget* GetInstance();
    static void DestroyInstance();
};