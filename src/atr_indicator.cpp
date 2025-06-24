#include "atr_indicator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

// --- Helper rolling mean/median implementations ---

std::vector<double> rolling_mean(const std::vector<double>& data, int window) {
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

std::vector<double> rolling_median(const std::vector<double>& data, int window, int min_periods) {
    std::vector<double> result(data.size(), std::nan(""));
    if (window <= 0 || data.empty()) return result;
    for (size_t i = 0; i < data.size(); ++i) {
        size_t start = (i + 1 >= static_cast<size_t>(window)) ? i + 1 - window : 0;
        size_t len = i + 1 - start;
        if (static_cast<int>(len) < min_periods) continue;
        std::vector<double> window_data(data.begin() + start, data.begin() + i + 1);
        std::vector<double> valid;
        for (auto v : window_data) if (!std::isnan(v)) valid.push_back(v);
        if (valid.empty()) continue;
        std::sort(valid.begin(), valid.end());
        size_t n = valid.size();
        if (n % 2 == 0)
            result[i] = (valid[n / 2 - 1] + valid[n / 2]) / 2.0;
        else
            result[i] = valid[n / 2];
    }
    return result;
}

// --- ATRIndicator implementation ---

ATRIndicator::ATRIndicator(const DataFrame& df, int window)
    : Indicator(df), window_(window)
{
    if (df_.close.size() < static_cast<size_t>(window_) + 1)
        throw std::invalid_argument("Insufficient data for ATRIndicator (window: " + std::to_string(window_) + "): "
            + std::to_string(df_.close.size()) + " rows provided, requires at least " + std::to_string(window_ + 1) + " rows.");

    atr_col_name_ = "ATR_" + std::to_string(window_);
    low_atr_signal_col_ = "ATR_Low_Signal_" + std::to_string(window_);
    high_atr_signal_col_ = "ATR_High_Signal_" + std::to_string(window_);

    signal_orientations_[low_atr_signal_col_] = "buy";
    signal_orientations_[high_atr_signal_col_] = "sell";
}

DataFrame ATRIndicator::calculate() {
    size_t n = df_.close.size();
    std::vector<double>& high = df_.high;
    std::vector<double>& low = df_.low;
    std::vector<double>& close = df_.close;
    std::vector<double> prev_close(n, std::nan(""));
    for (size_t i = 1; i < n; ++i)
        prev_close[i] = close[i - 1];

    // Calculate True Range (TR)
    std::vector<double> tr1(n), tr2(n), tr3(n), tr(n, std::nan(""));
    for (size_t i = 0; i < n; ++i) {
        tr1[i] = high[i] - low[i];
        tr2[i] = (i > 0) ? std::fabs(high[i] - prev_close[i]) : std::nan("");
        tr3[i] = (i > 0) ? std::fabs(low[i] - prev_close[i]) : std::nan("");
        tr[i] = std::max({tr1[i], tr2[i], tr3[i]});
    }

    // ATR as rolling mean of TR
    std::vector<double> atr = rolling_mean(tr, window_);

    // Add ATR column to DataFrame (add as an extra field for this indicator)
    df_.extra_columns[atr_col_name_] = atr;

    // Rolling median for dynamic threshold
    int rolling_median_window = std::max(1, window_ / 2);
    std::vector<double> atr_median = rolling_median(atr, window_, rolling_median_window);

    // Build signals: ATR below/above its rolling median
    std::vector<bool> low_signal(n, false), high_signal(n, false);
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(atr[i]) && !std::isnan(atr_median[i])) {
            low_signal[i] = (atr[i] < atr_median[i]);
            high_signal[i] = (atr[i] > atr_median[i]);
        }
    }
    df_.extra_bool_columns[low_atr_signal_col_] = low_signal;
    df_.extra_bool_columns[high_atr_signal_col_] = high_signal;

    return df_;
}
