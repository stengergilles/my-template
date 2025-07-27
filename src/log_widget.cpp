#include "log_widget.h"
#include "imgui.h"
#include "../include/state_manager.h" // Include StateManager
#include "../include/platform/logger.h" // Include logger.h
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>



LogWidget::LogWidget(size_t max_size, size_t max_lines) : max_size(max_size), max_lines(max_lines) {
    Buf = new char[max_size]; // Allocate buffer
    BufOffset = 0;
    ScrollToBottom = false;
    LineOffsets.push_back(0); // First line starts at offset 0
}

LogWidget::~LogWidget() {
    delete[] Buf; // Free buffer
}

void LogWidget::Clear() {
    std::lock_guard<std::mutex> lock(LogMutex);
    BufOffset = 0;
    LineOffsets.clear();
    LineOffsets.push_back(0);
}

void LogWidget::AddLogV(const char* fmt, va_list args) {
    std::lock_guard<std::mutex> lock(LogMutex);

    // Calculate the length of the formatted string
    va_list args_copy;
    va_copy(args_copy, args);
    int msg_len = vsnprintf(NULL, 0, fmt, args_copy); // Get length of formatted string
    va_end(args_copy);

    if (msg_len < 0) return; // Error

    // If message is too long for the entire buffer, truncate it
    if (msg_len >= max_size) {
        msg_len = max_size - 1; // Leave space for null terminator
    }

    // If adding the new message would exceed max_size, shift existing content
    if (BufOffset + msg_len + 1 > max_size) {
        int bytes_to_remove = (BufOffset + msg_len + 1) - max_size;
        // Find the first line that is completely removed by the shift
        int first_line_to_keep_idx = 0;
        for (size_t i = 0; i < LineOffsets.size(); ++i) {
            if (LineOffsets[i] >= bytes_to_remove) {
                first_line_to_keep_idx = i;
                break;
            }
        }

        // Shift the buffer content
        memmove(Buf, Buf + bytes_to_remove, BufOffset - bytes_to_remove);
        BufOffset -= bytes_to_remove;

        // Update LineOffsets
        for (size_t i = first_line_to_keep_idx; i < LineOffsets.size(); ++i) {
            LineOffsets[i] -= bytes_to_remove;
        }
        LineOffsets.erase(LineOffsets.begin(), LineOffsets.begin() + first_line_to_keep_idx);
    }

    // Write the message
    va_copy(args_copy, args);
    int written = vsnprintf(Buf + BufOffset, max_size - BufOffset, fmt, args_copy);
    va_end(args_copy);

    if (written < 0) return; // Error

    // Replace non-printable ASCII characters with space
    for (int i = 0; i < written; ++i) {
        if (Buf[BufOffset + i] < 32 || Buf[BufOffset + i] == 127) {
            Buf[BufOffset + i] = ' ';
        }
    }

    // Null-terminate the message
    Buf[BufOffset + written] = '\0';

    // Add new line offset
    LineOffsets.push_back(BufOffset);

    // Update BufOffset
    BufOffset += written + 1; // +1 for null terminator

    // If we've exceeded the max number of lines, remove the oldest
    while (LineOffsets.size() > max_lines) {
        LineOffsets.erase(LineOffsets.begin());
    }

    ScrollToBottom = true;
}

void LogWidget::AddLog(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddLogV(fmt, args);
    va_end(args);
}

void LogWidget::Draw(const char* title, bool* p_open) {
    
    // Load position
    float x = 0.0f, y = 0.0f;
    bool loaded_pos = StateManager::getInstance().loadWindowPosition(title, x, y);
    if (loaded_pos) {
        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
    }

    if (!ImGui::Begin(title, p_open)) {
        ImGui::End();
        return;
    }

    // Save position
    ImVec2 current_pos = ImGui::GetWindowPos();
    StateManager::getInstance().saveWindowPosition(title, current_pos.x, current_pos.y);

    // Options
    static bool auto_scroll = true;
    ImGui::Checkbox("Auto-scroll", &auto_scroll);
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        Clear();
    }

    ImGui::Separator();

    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    {
        std::lock_guard<std::mutex> lock(LogMutex);
        const char* buf_begin = Buf;
        const char* buf_end = Buf + BufOffset;
        for (int i = 0; i < LineOffsets.size(); i++) {
            const char* item = buf_begin + LineOffsets[i];
            ImGui::TextUnformatted(item);
        }
    }

    if (ScrollToBottom && auto_scroll) {
        ImGui::SetScrollHereY(1.0f);
    }
    ScrollToBottom = false;

    ImGui::EndChild();
    ImGui::End();
}

LogWidget* LogWidget::s_instance = nullptr;

LogWidget* LogWidget::GetInstance() {
    if (!s_instance) {
        s_instance = new LogWidget(2000, 500); // Default max_size and max_lines
    }
    return s_instance;
}

void LogWidget::DestroyInstance() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}
