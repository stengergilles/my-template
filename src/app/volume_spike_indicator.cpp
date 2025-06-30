#include "app/volume_spike_indicator.hpp"
#include "app/indicator_factory.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

VolumeSpikeIndicator::VolumeSpikeIndicator(const DataFrame& df,
                                           int window,
                                           double threshold,
                                           const std::string& column)
    : Indicator(df),
      window_(window),
      threshold_(threshold),
      column_(column)
{
    spike_col_ = "VolumeSpike_" + std::to_string(window_) + "_" + column_;
    buy_signal_col_ = spike_col_ + "_Buy";
    sell_signal_col_ = spike_col_ + "_Sell";

    signal_orientations_[buy_signal_col_] = "buy";
    signal_orientations_[sell_signal_col_] = "sell";
}

std::vector<double> VolumeSpikeIndicator::calculate_sma(const std::vector<double>& data, int window) {
    std::vector<double> sma(data.size(), std::nan(""));
    if (window <= 0 || static_cast<int>(data.size()) < window) return sma;
    double sum = 0.0;
    for (int i = 0; i < window; ++i) sum += data[i];
    sma[window - 1] = sum / window;
    for (size_t i = window; i < data.size(); ++i) {
        sum += data[i] - data[i - window];
        sma[i] = sum / window;
    }
    return sma;
}

DataFrame VolumeSpikeIndicator::calculate() {
    const std::vector<double>* volume_ptr = nullptr;
    if (column_ == "Volume") volume_ptr = &df_.volume;
    else if (column_ == "Close") volume_ptr = &df_.close;
    else if (column_ == "Open") volume_ptr = &df_.open;
    else if (column_ == "High") volume_ptr = &df_.high;
    else if (column_ == "Low") volume_ptr = &df_.low;
    else throw std::invalid_argument("Unknown column for VolumeSpike calculation: " + column_);
    const std::vector<double>& volume = *volume_ptr;
    size_t n = volume.size();

    std::vector<double> sma = calculate_sma(volume, window_);

    // Compute spike score: volume / sma
    std::vector<double> spike_score(n, std::nan(""));
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(sma[i]) && sma[i] > 0.0)
            spike_score[i] = volume[i] / sma[i];
    }

    // Buy signal: spike_score >= threshold
    // Sell signal: spike_score < 1/threshold (default: 0.5)
    std::vector<bool> buy_signal(n, false);
    std::vector<bool> sell_signal(n, false);
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(spike_score[i])) {
            if (spike_score[i] >= threshold_)
                buy_signal[i] = true;
            if (spike_score[i] < 1.0 / threshold_)
                sell_signal[i] = true;
        }
    }

    DataFrame out = df_;
    out.extra_columns[spike_col_] = spike_score;
    out.extra_bool_columns[buy_signal_col_] = buy_signal;
    out.extra_bool_columns[sell_signal_col_] = sell_signal;
    return out;
}

REGISTER_INDICATOR("VOLUMESPIKE",VolumeSpikeIndicator)