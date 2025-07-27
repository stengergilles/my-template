#include "write_cacert.hpp"
#include "cacert_pem_data.hpp"

#include <fstream>
#include <stdexcept>
#include <sys/stat.h>

const std::string& get_cacert_path() {
    static const std::string path = "cacert.pem";
    return path;
}

void write_cacert_pem_if_not_exists() {
    const std::string& path = get_cacert_path();
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return; // File exists
    }

    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open file for writing: " + path);
    out.write(reinterpret_cast<const char*>(cacert_pem), cacert_pem_len);
    out.close();
    if (!out) throw std::runtime_error("Failed to write cacert.pem to file: " + path);
}