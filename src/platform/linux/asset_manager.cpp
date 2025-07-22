#include "../../../include/platform/linux/asset_manager.h"
#include "../../../include/logger.h"
#include <algorithm>

// LinuxAsset implementation
int LinuxAsset::read(void* buf, size_t nbytes) {
    if (m_offset >= m_data.size()) {
        return 0; // End of file
    }
    size_t bytes_to_read = std::min(nbytes, m_data.size() - m_offset);
    std::copy_n(m_data.begin() + m_offset, bytes_to_read, static_cast<char*>(buf));
    m_offset += bytes_to_read;
    return static_cast<int>(bytes_to_read);
}

// LinuxAssetManager implementation
LinuxAssetManager::LinuxAssetManager(const std::string& base_path) : m_base_path(base_path) {
    LOG_INFO("LinuxAssetManager initialized with base path: %s", m_base_path.c_str());
}

std::shared_ptr<LinuxAsset> LinuxAssetManager::open(const std::string& filename) {
    std::string full_path = m_base_path + "/" + filename;
    LOG_INFO("Attempting to open asset: %s", full_path.c_str());
    std::ifstream file(full_path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        LOG_ERROR("Failed to open asset: %s", full_path.c_str());
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        LOG_ERROR("Failed to read asset: %s", full_path.c_str());
        return nullptr;
    }

    LOG_INFO("Successfully opened asset: %s, size: %zu bytes", full_path.c_str(), buffer.size());
    return std::make_shared<LinuxAsset>(std::move(buffer));
}
