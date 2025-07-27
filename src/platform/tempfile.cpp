#include "../include/platform/tempfile.hpp"
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#include <cstdio>

std::string portable_create_temp_file() {
    wchar_t tempPath[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, tempPath))
        throw std::runtime_error("GetTempPathW failed");
    wchar_t tempFile[MAX_PATH];
    if (!GetTempFileNameW(tempPath, L"cac", 0, tempFile))
        throw std::runtime_error("GetTempFileNameW failed");

    // Open and close the file to ensure it's created.
    FILE* file = _wfopen(tempFile, L"wb");
    if (!file)
        throw std::runtime_error("Failed to open temp file");
    fclose(file);

    // Convert wide string to UTF-8
    int len = WideCharToMultiByte(CP_UTF8, 0, tempFile, -1, NULL, 0, NULL, NULL);
    std::string out(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, tempFile, -1, &out[0], len, NULL, NULL);
    if (!out.empty() && out.back() == '\0') out.pop_back();
    return out;
}

#else // POSIX (Linux, macOS, Android, Emscripten)

#include <unistd.h>
#include <cstring>

std::string portable_create_temp_file() {
//    char tmpl[] = "/tmp/cacert-XXXXXX";
    char tmpl[] = "cacert-XXXXXX";
    int fd = mkstemp(tmpl);
    if (fd == -1)
        throw std::runtime_error("mkstemp failed: " + std::string(strerror(errno)));
    close(fd);
    return std::string(tmpl);
}
#endif
