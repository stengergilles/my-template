#include <gtest/gtest.h>
#include "atr_indicator.hpp"
#include <cmath>
#include <vector>
#include <string>

// Helper to generate a simple OHLCV DataFrame with an uptrend
DataFrame make_simple_ohlcv(size_t n) {
    DataFrame df;
    for (size_t i = 0; i < n; ++i) {
        df.open.push_back(100.0 + i);
        df.high.push_back(101.0 + i);
        df.low.push_back( 99.0 + i);
        df.close.push_back(100.5 + i);
        df.volume.push_back(1'000 + i * 10);
        df.datetime_index.push_back("2025-01-01T00:00:00Z"); // Dummy
    }
    return df;
}

TEST(ATRIndicatorTest, ThrowsOnTooShortData) {
    DataFrame df = make_simple_ohlcv(10); // Less than window+1
    EXPECT_THROW({
        ATRIndicator atr(df, 14);
    }, std::invalid_argument);
}

TEST(ATRIndicatorTest, BasicATRCalculation) {
    size_t n = 25;
    DataFrame df = make_simple_ohlcv(n);
    int window = 14;
    ATRIndicator atr(df, window);
    DataFrame out = atr.calculate();

    // ATR column should exist and be the right size
    auto it = out.extra_columns.find("ATR_14");
    ASSERT_NE(it, out.extra_columns.end());
    const std::vector<double>& atr_col = it->second;
    ASSERT_EQ(atr_col.size(), n);

    // All ATR values before window-1 should be NaN
    for (int i = 0; i < window-1; ++i) {
        EXPECT_TRUE(std::isnan(atr_col[i]));
    }

    // ATR values at or after window-1 should be finite (for this synthetic data)
    for (int i = window-1; i < (int)n; ++i) {
        EXPECT_FALSE(std::isnan(atr_col[i]));
        // For a perfect uptrend with constant range, ATR should be close to (high-low)=2.0
        EXPECT_NEAR(atr_col[i], 2.0, 1e-9);
    }
}

TEST(ATRIndicatorTest, SignalColumnsExist) {
    size_t n = 30;
    DataFrame df = make_simple_ohlcv(n);
    int window = 10;
    ATRIndicator atr(df, window);
    DataFrame out = atr.calculate();

    // Check signal columns exist and size matches data
    auto it_low = out.extra_bool_columns.find("ATR_Low_Signal_10");
    auto it_high = out.extra_bool_columns.find("ATR_High_Signal_10");
    ASSERT_NE(it_low, out.extra_bool_columns.end());
    ASSERT_NE(it_high, out.extra_bool_columns.end());
    EXPECT_EQ(it_low->second.size(), n);
    EXPECT_EQ(it_high->second.size(), n);

    // For uptrend with constant ATR, low and high signals should not both be true at the same index
    for (size_t i = 0; i < n; ++i) {
        EXPECT_FALSE(it_low->second[i] && it_high->second[i]);
    }
}

TEST(ATRIndicatorTest, SignalOrientations) {
    DataFrame df = make_simple_ohlcv(20);
    ATRIndicator atr(df, 5);
    auto orientations = atr.get_signal_orientations();
    EXPECT_EQ(orientations.at("ATR_Low_Signal_5"), "buy");
    EXPECT_EQ(orientations.at("ATR_High_Signal_5"), "sell");
}

