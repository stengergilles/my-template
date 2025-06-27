#include "volume_spike_indicator.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

TEST(VolumeSpikeIndicatorTest, CalculatesSpikeAndSignals) {
    // Data: mostly constant, with a spike in the middle and a drop at the end
    std::vector<double> volume = {
        100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
        450, // Big spike at index 10
        100, 100, 100, 100, 100, 100, 100, 100, 40  // Drop at end
    };
    std::vector<double> close(volume.size(), 10.0), open(volume.size(), 9.0), high(volume.size(), 11.0), low(volume.size(), 8.0);
    std::vector<std::string> dtidx(volume.size(), "2024-01-01T00:00:00Z");

    DataFrame df;
    df.open = open;
    df.high = high;
    df.low = low;
    df.close = close;
    df.volume = volume;
    df.datetime_index = dtidx;

    int window = 5;
    double threshold = 2.0;
    VolumeSpikeIndicator vspike(df, window, threshold, "Volume");
    DataFrame result = vspike.calculate();

    std::string spike_col = "VolumeSpike_5_Volume";
    std::string buy_col = spike_col + "_Buy";
    std::string sell_col = spike_col + "_Sell";

    const auto& spike_vals = result.extra_columns[spike_col];
    const auto& buy_signal = result.extra_bool_columns[buy_col];
    const auto& sell_signal = result.extra_bool_columns[sell_col];

    ASSERT_EQ(spike_vals.size(), volume.size());
    ASSERT_EQ(buy_signal.size(), volume.size());
    ASSERT_EQ(sell_signal.size(), volume.size());

    // Print for debugging
    std::cout << "idx\tvol\tspike\tbuy\tsell\n";
    for (size_t i = 0; i < volume.size(); ++i) {
        std::cout << i << "\t" << volume[i] << "\t" << spike_vals[i] << "\t" << buy_signal[i] << "\t" << sell_signal[i] << "\n";
    }

    // There should be at least one buy (the spike) and one sell (the drop)
    bool has_buy = false, has_sell = false;
    for (size_t i = 1; i < volume.size(); ++i) {
        has_buy |= buy_signal[i];
        has_sell |= sell_signal[i];
    }
    EXPECT_TRUE(has_buy);
    EXPECT_TRUE(has_sell);

    // At least one finite spike value after window
    int finite_count = 0;
    for (size_t i = window-1; i < volume.size(); ++i) {
        if (!std::isnan(spike_vals[i]))
            ++finite_count;
    }
    EXPECT_GT(finite_count, 0);
}
