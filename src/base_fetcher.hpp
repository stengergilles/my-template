#pragma once
#include <string>
#include <vector>
#include <stdexcept>

// A simple DataFrame-like structure for demonstration.
// In practice, you might use a library like Eigen, xtensor, or your own implementation.
struct DataFrame {
    // Columns: Open, High, Low, Close, Volume, DateTimeIndex
    std::vector<double> open;
    std::vector<double> high;
    std::vector<double> low;
    std::vector<double> close;
    std::vector<double> volume;
    std::vector<std::string> datetime_index;
};

class Fetcher {
public:
    virtual ~Fetcher() = default;

    // Fetches historical market data for a given identifier over a specified period and interval.
    virtual DataFrame fetch_data(const std::string& identifier,
                                 const std::string& period,
                                 const std::string& interval) = 0;

    // Returns the name of the fetching service (e.g., "CoinGecko", "Polygon.io").
    virtual std::string get_service_name() const = 0;
};
