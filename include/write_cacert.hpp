#pragma once
#include <string>

const std::string& get_cacert_path();
void write_cacert_pem_if_not_exists();