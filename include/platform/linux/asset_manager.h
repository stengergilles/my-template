#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>

// Mimics AAsset
class LinuxAsset {
public:
    LinuxAsset(std::vector<char> data) : m_data(std::move(data)), m_offset(0) {}

    const void* getBuffer() const { return m_data.data(); }
    size_t getLength() const { return m_data.size(); }
    int read(void* buf, size_t nbytes);
    void close() { /* No-op for now, as data is managed by shared_ptr */ }

private:
    std::vector<char> m_data;
    size_t m_offset;
};

// Mimics AAssetManager
class LinuxAssetManager {
public:
    LinuxAssetManager(const std::string& base_path);
    
    std::shared_ptr<LinuxAsset> open(const std::string& filename);

private:
    std::string m_base_path;
};
