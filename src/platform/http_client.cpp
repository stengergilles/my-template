#include "platform/http_client.hpp"
#include "platform/write_cacert.hpp"
#include "platform/logger.h" // Include logger.h
#include <curl/curl.h>
#include <sstream>
#include <stdexcept>
#include <cstdio> // for std::remove

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* str = static_cast<std::string*>(userp);
    size_t total_size = size * nmemb;
    str->append(static_cast<char*>(contents), total_size);
    return total_size;
}

HttpClient::HttpClient() {
    write_cacert_pem_if_not_exists();
    m_caBundlePath = get_cacert_path();
}

HttpClient::~HttpClient() {
    // The cacert.pem file is no longer a temporary file, so we don't remove it here.
}

HttpResponse HttpClient::get(const std::string& url,
                                    const std::map<std::string, std::string>& params,
                                    const std::map<std::string, std::string>& headers) {
    CURL* curl = curl_easy_init();
    std::string response_data;
    HttpResponse resp{0, ""};

    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Compose URL with query parameters
    std::ostringstream oss;
    oss << url;
    if (!params.empty()) {
        oss << "?";
        bool first = true;
        for (const auto& kv : params) {
            if (!first) oss << "&";
            first = false;
            char* key_escaped = curl_easy_escape(curl, kv.first.c_str(), 0);
            char* val_escaped = curl_easy_escape(curl, kv.second.c_str(), 0);
            oss << key_escaped << "=" << val_escaped;
            curl_free(key_escaped);
            curl_free(val_escaped);
        }
    }
    std::string full_url = oss.str();
    

    curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // Set headers
    struct curl_slist* chunk = nullptr;
    for (const auto& kv : headers) {
        std::string header_line = kv.first + ": " + kv.second;
        chunk = curl_slist_append(chunk, header_line.c_str());
    }
    if (chunk) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    // Set CAINFO from the path stored in the constructor
    curl_easy_setopt(curl, CURLOPT_CAINFO, m_caBundlePath.c_str());

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    resp.status_code = static_cast<int>(http_code);
    resp.text = response_data;

    if (chunk) {
        curl_slist_free_all(chunk);
    }
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res));
    }

    return resp;
}
