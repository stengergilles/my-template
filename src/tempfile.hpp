#pragma once
#include <string>

// Returns the path to a newly created unique temp file.
// Throws std::runtime_error on failure.
std::string portable_create_temp_file();
