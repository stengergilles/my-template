#pragma once

#if defined(__EMSCRIPTEN__)
    #include "http_client_wasm.hpp"
    using PlatformHttpClient = HttpClientWasm;
#else
    #include "platform/http_client.hpp"
    using PlatformHttpClient = HttpClient;
#endif
