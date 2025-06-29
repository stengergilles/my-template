#include "http_client_curl.hpp"
#include "write_cacert.hpp"
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

HttpClientLibcurl::HttpClientLibcurl() {}

HttpResponse HttpClientLibcurl::get(const std::string& url,
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
        curl_slist_free_all(chunk);
    }

    // Write the embedded cacert.pem to a temp file and set CAINFO
    std::string ca_bundle_path = write_cacert_pem_to_tempfile();
    curl_easy_setopt(curl, CURLOPT_CAINFO, ca_bundle_path.c_str());

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    resp.status_code = static_cast<int>(http_code);
    resp.text = response_data;

    if (chunk) {
        curl_slist_free_all(chunk);
    }
    curl_easy_cleanup(curl);

    // Clean up temporary file
    std::remove(ca_bundle_path.c_str());

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res));
    }

    return resp;
}