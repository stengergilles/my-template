#pragma once
#include <string>

// Writes the embedded cacert_pem[] to a new temp file and returns its path.
std::string write_cacert_pem_to_tempfile();
