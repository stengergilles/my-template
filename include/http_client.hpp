#pragma once
#include <string>
#include <map>

// Simple HTTP response struct
struct HttpResponse {
    int status_code;
    std::string text;
};

// Abstract interface for platform HTTP clients
class HttpClient {
public:
    virtual ~HttpClient() = default;
    virtual HttpResponse get(const std::string& url,
                             const std::map<std::string, std::string>& params,
                             const std::map<std::string, std::string>& headers) = 0;
};
