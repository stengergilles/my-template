#include "macd_indicator.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

// Helper: Skips leading nans for EMA calculation
std::vector<double> MACDIndicator::calculate_ema(const std::vector<double>& data, int period) {
    std::vector<double> result(data.size(), std::nan(""));
    if (period <= 0) return result;

    // Find the first non-nan index
    size_t start = 0;
    while (start < data.size() && std::isnan(data[start])) ++start;
    if (data.size() - start < static_cast<size_t>(period)) return result;

    double alpha = 2.0 / (period + 1.0);
    double ema = 0.0;

    // Initialize EMA with simple average of the first 'period' values (starting from 'start')
    double sum = 0.0;
    for (int j = 0; j < period; ++j) sum += data[start + j];
    ema = sum / period;
    result[start + period - 1] = ema;

    // Continue EMA calculation
    for (size_t i = start + period; i < data.size(); ++i) {
        ema = alpha * data[i] + (1.0 - alpha) * ema;
        result[i] = ema;
    }
    return result;
}

MACDIndicator::MACDIndicator(const DataFrame& df,
                             int fast_period,
                             int slow_period,
                             int signal_period,
                             const std::string& column)
    : Indicator(df),
      fast_period_(fast_period),
      slow_period_(slow_period),
      signal_period_(signal_period),
      column_(column)
{
    macd_col_ = "MACD_" + std::to_string(fast_period_) + "_" + std::to_string(slow_period_) + "_" + column_;
    signal_col_ = "MACDSignal_" + std::to_string(signal_period_) + "_" + column_;
    hist_col_ = "MACDHist_" + std::to_string(fast_period_) + "_" + std::to_string(slow_period_) + "_" + std::to_string(signal_period_) + "_" + column_;
    buy_signal_col_ = macd_col_ + "_Cross_Above_Signal";
    sell_signal_col_ = macd_col_ + "_Cross_Below_Signal";

    signal_orientations_[buy_signal_col_] = "buy";
    signal_orientations_[sell_signal_col_] = "sell";
}

DataFrame MACDIndicator::calculate() {
    const std::vector<double>* price_ptr = nullptr;
    if (column_ == "Close") price_ptr = &df_.close;
    else if (column_ == "Open") price_ptr = &df_.open;
    else if (column_ == "High") price_ptr = &df_.high;
    else if (column_ == "Low") price_ptr = &df_.low;
    else if (column_ == "Volume") price_ptr = &df_.volume;
    else throw std::invalid_argument("Unknown column for MACD calculation: " + column_);
    const std::vector<double>& price = *price_ptr;
    size_t n = price.size();

    std::vector<double> fast_ema = calculate_ema(price, fast_period_);
    std::vector<double> slow_ema = calculate_ema(price, slow_period_);
    std::vector<double> macd(n, std::nan(""));
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(fast_ema[i]) && !std::isnan(slow_ema[i]))
            macd[i] = fast_ema[i] - slow_ema[i];
    }

    // Signal line: use new EMA logic that skips leading nans
    std::vector<double> signal = calculate_ema(macd, signal_period_);
    std::vector<double> hist(n, std::nan(""));
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(macd[i]) && !std::isnan(signal[i]))
            hist[i] = macd[i] - signal[i];
    }

    std::vector<bool> buy_signal(n, false);
    std::vector<bool> sell_signal(n, false);

    for (size_t i = 1; i < n; ++i) {
        if (!std::isnan(macd[i]) && !std::isnan(signal[i]) && !std::isnan(macd[i-1]) && !std::isnan(signal[i-1])) {
            // MACD crosses above signal
            if (macd[i] > signal[i] && macd[i-1] <= signal[i-1])
                buy_signal[i] = true;
            // MACD crosses below signal
            if (macd[i] < signal[i] && macd[i-1] >= signal[i-1])
                sell_signal[i] = true;
        }
    }

    DataFrame out = df_;
    out.extra_columns[macd_col_] = macd;
    out.extra_columns[signal_col_] = signal;
    out.extra_columns[hist_col_] = hist;
    out.extra_bool_columns[buy_signal_col_] = buy_signal;
    out.extra_bool_columns[sell_signal_col_] = sell_signal;
    return out;
}
