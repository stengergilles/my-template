#pragma once
#include "http_client.hpp"

class HttpClientLibcurl : public HttpClient {
public:
    HttpClientLibcurl();
    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& params,
                     const std::map<std::string, std::string>& headers) override;
};
