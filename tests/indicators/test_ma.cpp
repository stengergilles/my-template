#include "ma_indicator.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cmath>

TEST(MAIndicatorTest, CalculatesSMAAndSignals) {
    // Data oscillates around SMA
    // SMA(3): [nan, nan, 2.0, 3.0, 3.333..., 4.0]
    // Close:  [1,2,3,4,3,5]
    // Cross above expected at index 5 (3->5, crosses above 4.0)
    // Cross below expected at index 4 (4->3, crosses below 3.333...)
    std::vector<double> close = {1,2,3,4,3,5};
    std::vector<double> open = close, high = close, low = close, volume(close.size(), 1000.0);
    std::vector<std::string> dtidx(close.size(), "2024-01-01T00:00:00Z");

    DataFrame df;
    df.open = open;
    df.high = high;
    df.low = low;
    df.close = close;
    df.volume = volume;
    df.datetime_index = dtidx;

    int window = 3;
    MAIndicator ma(df, window, "sma", "Close");
    DataFrame result = ma.calculate();

    std::string ma_col = "SMA_3_Close";
    std::string buy_col = "SMA_3_Close_Cross_Above";
    std::string sell_col = "SMA_3_Close_Cross_Below";

    const auto& sma = result.extra_columns[ma_col];
    const auto& buy_signal = result.extra_bool_columns[buy_col];
    const auto& sell_signal = result.extra_bool_columns[sell_col];

    // SMA checks
    ASSERT_EQ(sma.size(), close.size());
    EXPECT_TRUE(std::isnan(sma[0]));
    EXPECT_TRUE(std::isnan(sma[1]));
    EXPECT_NEAR(sma[2], 2.0, 1e-5);
    EXPECT_NEAR(sma[3], 3.0, 1e-5);
    EXPECT_NEAR(sma[4], 3.333333, 1e-5);
    EXPECT_NEAR(sma[5], 4.0, 1e-5);

    // Cross above at index 5
    EXPECT_TRUE(buy_signal[5]);
    // Cross below at index 4
    EXPECT_TRUE(sell_signal[4]);
    // All other indices should be false for both signals
    for (size_t i = 0; i < close.size(); ++i) {
        if (i != 5) EXPECT_FALSE(buy_signal[i]);
        if (i != 4) EXPECT_FALSE(sell_signal[i]);
    }
}

TEST(MAIndicatorTest, CalculatesEMAAndSignals) {
    // Data oscillates around EMA
    // Use: [10, 8, 6, 8, 6, 8]
    // Should produce at least one cross above and one cross below
    std::vector<double> close = {10, 8, 6, 8, 6, 8};
    std::vector<double> open = close, high = close, low = close, volume(close.size(), 1000.0);
    std::vector<std::string> dtidx(close.size(), "2024-01-01T00:00:00Z");

    DataFrame df;
    df.open = open;
    df.high = high;
    df.low = low;
    df.close = close;
    df.volume = volume;
    df.datetime_index = dtidx;

    int window = 3;
    MAIndicator ma(df, window, "ema", "Close");
    DataFrame result = ma.calculate();

    std::string ma_col = "EMA_3_Close";
    std::string buy_col = "EMA_3_Close_Cross_Above";
    std::string sell_col = "EMA_3_Close_Cross_Below";

    const auto& ema = result.extra_columns[ma_col];
    const auto& buy_signal = result.extra_bool_columns[buy_col];
    const auto& sell_signal = result.extra_bool_columns[sell_col];

    ASSERT_EQ(ema.size(), close.size());
    ASSERT_EQ(buy_signal.size(), close.size());
    ASSERT_EQ(sell_signal.size(), close.size());

    // At least one cross above and one cross below (due to oscillation)
    bool has_buy = false, has_sell = false;
    for (size_t i = 0; i < close.size(); ++i) {
        has_buy |= buy_signal[i];
        has_sell |= sell_signal[i];
    }
    EXPECT_TRUE(has_buy);
    EXPECT_TRUE(has_sell);
}
