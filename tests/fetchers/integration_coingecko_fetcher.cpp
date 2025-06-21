#include <gtest/gtest.h>
#include "coingecko_fetcher.hpp"
#include <iostream>

// Helper to check DataFrame columns (adapt to your DataFrame interface)
bool has_column(const DataFrame& df, const std::string& col_name) {
    auto cols = df.columns();
    return std::find(cols.begin(), cols.end(), col_name) != cols.end();
}

// Integration test for CoinGeckoFetcher using real API
TEST(CoinGeckoFetcherIntegrationTest, FetchBitcoin1d) {
    CoinGeckoFetcher fetcher; // No API key needed for free endpoints

    try {
        DataFrame df = fetcher.fetch_data("bitcoin", "1d", "daily");

        // Print for debug
        std::cout << "Fetched DataFrame rows: " << df.size() << std::endl;

        // Basic checks
        ASSERT_GT(df.size(), 0) << "DataFrame should have at least 1 row.";
        ASSERT_TRUE(has_column(df, "Timestamp")) << "DataFrame must have 'Timestamp' column.";
        ASSERT_TRUE(has_column(df, "Close")) << "DataFrame must have 'Close' column.";
        // Optionally check for other expected columns
        ASSERT_TRUE(has_column(df, "Open"));
        ASSERT_TRUE(has_column(df, "High"));
        ASSERT_TRUE(has_column(df, "Low"));
        ASSERT_TRUE(has_column(df, "Volume"));

        // Optionally, print the first row for manual inspection
        // auto first_row = df.row(0);
        // for (const auto& col : df.columns()) {
        //     std::cout << col << ": " << first_row[col] << " ";
        // }
        // std::cout << std::endl;

    } catch (const std::exception& ex) {
        FAIL() << "Exception thrown during fetch_data: " << ex.what();
    }
}
