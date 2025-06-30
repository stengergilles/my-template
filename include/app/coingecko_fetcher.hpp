#pragma once
#include "base_fetcher.hpp"
#include <string>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <iostream>
#include <nlohmann/json.hpp> // You need https://github.com/nlohmann/json
// For HTTP requests, you can use cpr (https://github.com/libcpr/cpr) or libcurl wrapper

class CoinGeckoFetcher : public Fetcher {
public:
    CoinGeckoFetcher(const std::string& api_key = "");

    DataFrame fetch_data(const std::string& identifier,
                        const std::string& period,
                        const std::string& interval) override;

    std::string get_service_name() const override;

private:
    std::string api_key_;

    static std::string to_lower(const std::string& s);
    static bool is_number(const std::string& s);
    static double random_uniform();
    static std::string timestamp_to_iso8601(int64_t ms_since_epoch);

    // Returns the raw JSON string from CoinGecko (separates fetch from parse)
    std::string fetch_raw_response(const std::string& identifier,
                                   const std::string& period,
                                   const std::string& interval);
};
