#pragma once

#if defined(__EMSCRIPTEN__)
    #include "http_client_wasm.hpp"
    using PlatformHttpClient = HttpClientWasm;
#else
    #include "http_client_curl.hpp"
    using PlatformHttpClient = HttpClientLibcurl;
#endif
