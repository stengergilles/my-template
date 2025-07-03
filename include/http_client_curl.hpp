#pragma once
#include "http_client.hpp"
#include <string>

class HttpClientLibcurl : public HttpClient {
public:
    HttpClientLibcurl();
    ~HttpClientLibcurl();
    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& params,
                     const std::map<std::string, std::string>& headers) override;

private:
    std::string m_caBundlePath;
};
