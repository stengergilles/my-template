#include <gtest/gtest.h>
#include "bollinger_bands_indicator.hpp"
#include "base_fetcher.hpp"  // for DataFrame

TEST(BollingerBandsIndicatorTest, CalculatesBandsAndSignalsCorrectly) {
    DataFrame df;
    df.close = {10, 12, 13, 12, 14, 15, 17, 19, 18, 20};
    df.open = df.high = df.low = df.volume = std::vector<double>(df.close.size(), 0.0);
    df.datetime_index = std::vector<std::string>(df.close.size(), "2024-01-01T00:00:00Z");

    BollingerBandsIndicator indicator(df, 3, 1.0);
    DataFrame result = indicator.calculate();

    std::string middle = "BB_Middle_3";
    std::string upper = "BB_Upper_3";
    std::string lower = "BB_Lower_3";
    std::string buy = "BB_Buy_Signal_3";
    std::string sell = "BB_Sell_Signal_3";

    ASSERT_EQ(result.extra_columns[middle].size(), df.close.size());
    ASSERT_EQ(result.extra_columns[upper].size(), df.close.size());
    ASSERT_EQ(result.extra_columns[lower].size(), df.close.size());
    EXPECT_TRUE(std::isnan(result.extra_columns[middle][0]));
    EXPECT_TRUE(std::isnan(result.extra_columns[middle][1]));

    ASSERT_EQ(result.extra_bool_columns[buy].size(), df.close.size());
    ASSERT_EQ(result.extra_bool_columns[sell].size(), df.close.size());

    for (size_t i = 0; i < result.extra_bool_columns[buy].size(); ++i) {
        EXPECT_FALSE(result.extra_bool_columns[buy][i]);
    }
    // Optionally add further checks for sell signals or exact band values.
}

TEST(BollingerBandsIndicatorTest, ThrowsOnInsufficientData) {
    DataFrame df;
    df.close = {10, 12}; // Less than window size
    df.open = df.high = df.low = df.volume = std::vector<double>(df.close.size(), 0.0);
    df.datetime_index = std::vector<std::string>(df.close.size(), "2024-01-01T00:00:00Z");

    EXPECT_THROW({
        BollingerBandsIndicator indicator(df, 3, 1.0);
    }, std::invalid_argument);
}
