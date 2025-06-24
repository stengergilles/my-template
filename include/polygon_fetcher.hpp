#pragma once

#include "base_fetcher.hpp"
#include "http_client.hpp"
#include "platform_http_client.hpp"
#include <string>
#include <map>

class PolygonFetcher : public Fetcher {
public:
    explicit PolygonFetcher(const std::string& api_key);

    DataFrame fetch_data(const std::string& identifier,
                        const std::string& period,
                        const std::string& interval) override;

    std::string get_service_name() const override;

private:
    std::string api_key_;

    static std::pair<std::string, std::string> resolve_dates(const std::string& period);
    static std::pair<int, std::string> map_interval(const std::string& interval);
    static std::string strftime_date(const std::tm& tm);
};
