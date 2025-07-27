#pragma once
#include <string>
#include <map>

struct HttpResponse {
    int status_code;
    std::string text;
};

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual HttpResponse get(const std::string& url,
                             const std::map<std::string, std::string>& params,
                             const std::map<std::string, std::string>& headers) = 0;
};

class HttpClient : public IHttpClient {
public:
    HttpClient();
    ~HttpClient();
    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& params,
                     const std::map<std::string, std::string>& headers) override;

private:
    std::string m_caBundlePath;
};