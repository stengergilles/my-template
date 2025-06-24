#include "bollinger_bands_indicator.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

// Helper: rolling mean
static std::vector<double> rolling_mean(const std::vector<double>& data, int window) {
    std::vector<double> result(data.size(), std::nan(""));
    if (window <= 0 || data.size() < static_cast<size_t>(window)) return result;
    double sum = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
        if (i >= static_cast<size_t>(window)) sum -= data[i - window];
        if (i + 1 >= static_cast<size_t>(window))
            result[i] = sum / window;
    }
    return result;
}

// Helper: rolling stddev
static std::vector<double> rolling_std(const std::vector<double>& data, int window) {
    std::vector<double> result(data.size(), std::nan(""));
    if (window <= 0 || data.size() < static_cast<size_t>(window)) return result;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i + 1 < static_cast<size_t>(window)) continue;
        size_t start = i + 1 - window;
        double mean = std::accumulate(data.begin() + start, data.begin() + i + 1, 0.0) / window;
        double sum_sq = 0.0;
        for (size_t j = start; j <= i; ++j) {
            double diff = data[j] - mean;
            sum_sq += diff * diff;
        }
        result[i] = std::sqrt(sum_sq / window);
    }
    return result;
}

BollingerBandsIndicator::BollingerBandsIndicator(const DataFrame& df, int window, double num_std)
    : Indicator(df), window_(window), num_std_(num_std)
{
    if (df_.close.size() < static_cast<size_t>(window_) + 1)
        throw std::invalid_argument("Insufficient data for BollingerBandsIndicator (window: " + std::to_string(window_) + "): "
            + std::to_string(df_.close.size()) + " rows provided, requires at least " + std::to_string(window_ + 1) + " rows.");

    bb_middle_col_ = "BB_Middle_" + std::to_string(window_);
    bb_upper_col_ = "BB_Upper_" + std::to_string(window_);
    bb_lower_col_ = "BB_Lower_" + std::to_string(window_);
    signal_buy_col_ = "BB_Buy_Signal_" + std::to_string(window_);
    signal_sell_col_ = "BB_Sell_Signal_" + std::to_string(window_);

    signal_orientations_[signal_buy_col_] = "buy";
    signal_orientations_[signal_sell_col_] = "sell";
}

DataFrame BollingerBandsIndicator::calculate() {
    size_t n = df_.close.size();
    const std::vector<double>& close = df_.close;

    // Rolling mean and stddev of close
    std::vector<double> ma = rolling_mean(close, window_);
    std::vector<double> stds = rolling_std(close, window_);

    // Calculate bands
    std::vector<double> upper(n, std::nan(""));
    std::vector<double> lower(n, std::nan(""));
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(ma[i]) && !std::isnan(stds[i])) {
            upper[i] = ma[i] + num_std_ * stds[i];
            lower[i] = ma[i] - num_std_ * stds[i];
        }
    }

    df_.extra_columns[bb_middle_col_] = ma;
    df_.extra_columns[bb_upper_col_] = upper;
    df_.extra_columns[bb_lower_col_] = lower;

    // Generate buy/sell signals:
    std::vector<bool> buy_sig(n, false), sell_sig(n, false);
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(lower[i]) && close[i] < lower[i]) buy_sig[i] = true;
        if (!std::isnan(upper[i]) && close[i] > upper[i]) sell_sig[i] = true;
    }
    df_.extra_bool_columns[signal_buy_col_] = buy_sig;
    df_.extra_bool_columns[signal_sell_col_] = sell_sig;

    return df_;
}
