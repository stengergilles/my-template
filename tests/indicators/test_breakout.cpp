#include "breakout_indicator.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cmath>

TEST(BreakoutIndicatorTest, DetectsBreakoutsCorrectly) {
    // Simulated OHLCV data (20-bar window)
    std::vector<double> high   = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
    std::vector<double> low    = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    // At index 20: close (21) > max(high[0:20]) = 20 (bullish breakout)
    // At index 21: close (22) > max(high[1:21]) = 21 (bullish breakout)
    // At index 22: close (1) < min(low[2:22]) = 2 (bearish breakout)
    std::vector<double> close  = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,1};
    std::vector<double> open   = high; // dummy
    std::vector<double> volume(high.size(), 1000.0);
    std::vector<std::string> dtidx(high.size(), "2024-01-01T00:00:00Z");

    size_t n = high.size();

    DataFrame df;
    df.open = open;
    df.high = high;
    df.low = low;
    df.close = close;
    df.volume = volume;
    df.datetime_index = dtidx;

    BreakoutIndicator breakout(df, 20);
    DataFrame result = breakout.calculate();

    std::string bull_col = "Breakout_Bullish_Signal_20";
    std::string bear_col = "Breakout_Bearish_Signal_20";

    ASSERT_EQ(result.extra_bool_columns[bull_col].size(), n);
    ASSERT_EQ(result.extra_bool_columns[bear_col].size(), n);

    for (size_t i = 0; i < n; ++i) {
        if (i == 20) {
            EXPECT_TRUE(result.extra_bool_columns[bull_col][i]);   // bullish breakout
            EXPECT_FALSE(result.extra_bool_columns[bear_col][i]);
        } else if (i == 21) {
            EXPECT_TRUE(result.extra_bool_columns[bull_col][i]);   // bullish breakout
            EXPECT_FALSE(result.extra_bool_columns[bear_col][i]);
        } else if (i == 22) {
            EXPECT_FALSE(result.extra_bool_columns[bull_col][i]);
            EXPECT_TRUE(result.extra_bool_columns[bear_col][i]);   // bearish breakout
        } else {
            EXPECT_FALSE(result.extra_bool_columns[bull_col][i]);
            EXPECT_FALSE(result.extra_bool_columns[bear_col][i]);
        }
    }
}
