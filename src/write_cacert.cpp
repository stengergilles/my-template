#include "write_cacert.hpp"
#include "tempfile.hpp"
#include "cacert_pem_data.hpp"

#include <fstream>
#include <stdexcept>

std::string write_cacert_pem_to_tempfile() {
    std::string temp_path = portable_create_temp_file();
    std::ofstream out(temp_path, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open temp file for writing");
    out.write(reinterpret_cast<const char*>(cacert_pem), cacert_pem_len);
    out.close();
    if (!out) throw std::runtime_error("Failed to write cacert.pem to temp file");
    return temp_path;
}
