#include "rsi_indicator.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

TEST(RSIIndicatorTest, CalculatesRSIAndSignals) {
    // Force RSI below 30 and above 70 for clear signal crossings
    std::vector<double> close = {100, 80, 60, 40, 20, 40, 60, 80, 100, 80, 60, 40, 20, 40, 60, 80, 100};
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
    RSIIndicator rsi(df, window, "Close");
    DataFrame result = rsi.calculate();

    std::string rsi_col = "RSI_3_Close";
    std::string buy_col = "RSI_3_Close_Cross_Above_30";
    std::string sell_col = "RSI_3_Close_Cross_Below_70";

    const auto& rsi_vals = result.extra_columns[rsi_col];
    const auto& buy_signal = result.extra_bool_columns[buy_col];
    const auto& sell_signal = result.extra_bool_columns[sell_col];

    ASSERT_EQ(rsi_vals.size(), close.size());
    ASSERT_EQ(buy_signal.size(), close.size());
    ASSERT_EQ(sell_signal.size(), close.size());

    std::cout << "idx\tclose\trsi\tbuy\tsell\n";
    for (size_t i = 0; i < close.size(); ++i) {
        std::cout << i << "\t" << close[i] << "\t" << rsi_vals[i] << "\t" << buy_signal[i] << "\t" << sell_signal[i] << "\n";
    }

    // There should be at least one cross above 30 (buy) and one cross below 70 (sell)
    bool has_buy = false, has_sell = false;
    for (size_t i = 1; i < close.size(); ++i) {
        has_buy |= buy_signal[i];
        has_sell |= sell_signal[i];
    }
    EXPECT_TRUE(has_buy);
    EXPECT_TRUE(has_sell);

    int finite_count = 0;
    for (size_t i = window; i < close.size(); ++i) {
        if (!std::isnan(rsi_vals[i]))
            ++finite_count;
    }
    EXPECT_GT(finite_count, 0);
}
