#pragma once

#include <string>
#include <map>

struct HttpResponse {
    int status_code;
    std::string text;
};

class HttpClient {
public:
    virtual ~HttpClient() = default;
    virtual HttpResponse get(const std::string& url,
                             const std::map<std::string, std::string>& params,
                             const std::map<std::string, std::string>& headers) = 0;
};

// Use platform-specific implementations
#if defined(__EMSCRIPTEN__)
// WASM: Use Emscripten fetch API (to be implemented separately)
#include "http_client_wasm.hpp"
using PlatformHttpClient = HttpClientWasm;
#else
// Desktop/mobile: Use curl
#include "http_client_curl.hpp"
using PlatformHttpClient = HttpClientLibcurl;
#endif
