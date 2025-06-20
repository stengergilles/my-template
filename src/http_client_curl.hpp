#pragma once
#include "http_client.hpp"
#include <curl/curl.h>
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>

class HttpClientLibcurl : public HttpClient {
public:
    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& params,
                     const std::map<std::string, std::string>& headers) override 
    {
        // Build the query string
        std::string full_url = url;
        if (!params.empty()) {
            std::ostringstream oss;
            bool first = true;
            for (const auto& kv : params) {
                if (!first) oss << "&";
                oss << curl_easy_escape(nullptr, kv.first.c_str(), 0)
                    << "="
                    << curl_easy_escape(nullptr, kv.second.c_str(), 0);
                first = false;
            }
            full_url += (url.find('?') == std::string::npos ? "?" : "&") + oss.str();
        }

        CURL* curl = curl_easy_init();
        if (!curl) throw std::runtime_error("Failed to init curl");

        std::string response_string;
        long http_code = 0;

        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClientLibcurl::write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        // Enable HTTPS verification (recommended for security)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

        // Set headers
        struct curl_slist* chunk = nullptr;
        for (const auto& kv : headers) {
            std::string header = kv.first + ": " + kv.second;
            chunk = curl_slist_append(chunk, header.c_str());
        }
        if (chunk) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        }

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        } else {
            http_code = 0;
        }

        if (chunk) {
            curl_slist_free_all(chunk);
        }
        curl_easy_cleanup(curl);

        return HttpResponse{static_cast<int>(http_code), response_string};
    }

private:
    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t realsize = size * nmemb;
        std::string* out = static_cast<std::string*>(userp);
        out->append(static_cast<char*>(contents), realsize);
        return realsize;
    }
};
