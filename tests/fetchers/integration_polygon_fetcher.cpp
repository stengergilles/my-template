#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <iostream>
#include "polygon_fetcher.hpp"

// Helper function to read API key from environment or fail the test
std::string get_polygon_api_key() {
    const char* key = std::getenv("POLYGON_API_KEY");
    if (!key) {
	throw std::invalid_argument("POLYGON_API_KEY environment variable not set");
    }
    return std::string(key);
}

TEST(PolygonFetcherIntegration, FetchDailyBars) {
    std::string api_key = get_polygon_api_key();
    PolygonFetcher fetcher(api_key);

    std::string symbol = "AAPL";
    std::string period = "5d";
    std::string interval = "1d";

    DataFrame df = fetcher.fetch_data(symbol, period, interval);

    // Check nonzero rows
    ASSERT_GT(df.size(), 0u);

    // Check all columns have same size as index
    ASSERT_EQ(df.open.size(), df.datetime_index.size());
    ASSERT_EQ(df.high.size(), df.datetime_index.size());
    ASSERT_EQ(df.low.size(), df.datetime_index.size());
    ASSERT_EQ(df.close.size(), df.datetime_index.size());
    ASSERT_EQ(df.volume.size(), df.datetime_index.size());

    // Print the first row for inspection
    std::cout << "First row: "
              << "Timestamp=" << df.datetime_index[0]
              << ", Open=" << df.open[0]
              << ", High=" << df.high[0]
              << ", Low=" << df.low[0]
              << ", Close=" << df.close[0]
              << ", Volume=" << df.volume[0]
              << std::endl;
}

TEST(PolygonFetcherIntegration, ServiceName) {
    std::string api_key = get_polygon_api_key();
    PolygonFetcher fetcher(api_key);
    ASSERT_EQ(fetcher.get_service_name(), "Polygon.io");
}
