#include "macd_indicator.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

TEST(MACDIndicatorTest, CalculatesMACDAndSignals) {
    // Strongly oscillating data and long enough for MACD and Signal EMAs
    std::vector<double> close = {
        1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5
    };
    std::vector<double> open = close, high = close, low = close, volume(close.size(), 1000.0);
    std::vector<std::string> dtidx(close.size(), "2024-01-01T00:00:00Z");

    DataFrame df;
    df.open = open;
    df.high = high;
    df.low = low;
    df.close = close;
    df.volume = volume;
    df.datetime_index = dtidx;

    int fast = 2, slow = 4, signal = 2;
    MACDIndicator macd(df, fast, slow, signal, "Close");
    DataFrame result = macd.calculate();

    std::string macd_col = "MACD_2_4_Close";
    std::string signal_col = "MACDSignal_2_Close";
    std::string hist_col = "MACDHist_2_4_2_Close";
    std::string buy_col = "MACD_2_4_Close_Cross_Above_Signal";
    std::string sell_col = "MACD_2_4_Close_Cross_Below_Signal";

    const auto& macd_vals = result.extra_columns[macd_col];
    const auto& signal_vals = result.extra_columns[signal_col];
    const auto& hist_vals = result.extra_columns[hist_col];
    const auto& buy_signal = result.extra_bool_columns[buy_col];
    const auto& sell_signal = result.extra_bool_columns[sell_col];

    ASSERT_EQ(macd_vals.size(), close.size());
    ASSERT_EQ(signal_vals.size(), close.size());
    ASSERT_EQ(hist_vals.size(), close.size());
    ASSERT_EQ(buy_signal.size(), close.size());
    ASSERT_EQ(sell_signal.size(), close.size());

    // Print for debugging
    std::cout << "idx\tclose\tmacd\tsignal\thist\tbuy\tsell\n";
    for (size_t i = 0; i < close.size(); ++i) {
        std::cout << i << "\t" << close[i] << "\t"
                  << macd_vals[i] << "\t"
                  << signal_vals[i] << "\t"
                  << hist_vals[i] << "\t"
                  << buy_signal[i] << "\t"
                  << sell_signal[i] << "\n";
    }

    // There should be at least one cross above and one cross below
    bool has_buy = false, has_sell = false;
    for (size_t i = 1; i < close.size(); ++i) {
        has_buy |= buy_signal[i];
        has_sell |= sell_signal[i];
    }
    EXPECT_TRUE(has_buy);
    EXPECT_TRUE(has_sell);

    // MACD, Signal, and Hist columns should be finite (not nan) after enough data
    int min_period = std::max({fast, slow, signal});
    int finite_count = 0;
    for (size_t i = min_period+1; i < close.size(); ++i) {
        if (!std::isnan(macd_vals[i]) && !std::isnan(signal_vals[i]) && !std::isnan(hist_vals[i]))
            ++finite_count;
    }
    EXPECT_GT(finite_count, 0);
}
