#include "app/rsi_indicator.hpp"
#include "app/indicator_factory.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

RSIIndicator::RSIIndicator(const DataFrame& df,
                           int window,
                           const std::string& column)
    : Indicator(df),
      window_(window),
      column_(column)
{
    rsi_col_ = "RSI_" + std::to_string(window_) + "_" + column_;
    buy_signal_col_ = rsi_col_ + "_Cross_Above_30";
    sell_signal_col_ = rsi_col_ + "_Cross_Below_70";

    signal_orientations_[buy_signal_col_] = "buy";
    signal_orientations_[sell_signal_col_] = "sell";
}

std::vector<double> RSIIndicator::calculate_rsi(const std::vector<double>& data, int window) {
    std::vector<double> rsi(data.size(), std::nan(""));
    if (window <= 0 || static_cast<int>(data.size()) <= window) return rsi;

    std::vector<double> gains(data.size(), 0.0);
    std::vector<double> losses(data.size(), 0.0);

    for (size_t i = 1; i < data.size(); ++i) {
        double diff = data[i] - data[i - 1];
        gains[i] = diff > 0 ? diff : 0.0;
        losses[i] = diff < 0 ? -diff : 0.0;
    }

    double avg_gain = 0.0, avg_loss = 0.0;
    for (int i = 1; i <= window; ++i) {
        avg_gain += gains[i];
        avg_loss += losses[i];
    }
    avg_gain /= window;
    avg_loss /= window;

    // First RSI value at window-th index
    if (avg_loss == 0.0)
        rsi[window] = 100.0;
    else {
        double rs = avg_gain / avg_loss;
        rsi[window] = 100.0 - (100.0 / (1.0 + rs));
    }

    // Smoothed RSI
    for (size_t i = window + 1; i < data.size(); ++i) {
        avg_gain = (avg_gain * (window - 1) + gains[i]) / window;
        avg_loss = (avg_loss * (window - 1) + losses[i]) / window;

        if (avg_loss == 0.0)
            rsi[i] = 100.0;
        else {
            double rs = avg_gain / avg_loss;
            rsi[i] = 100.0 - (100.0 / (1.0 + rs));
        }
    }
    return rsi;
}

DataFrame RSIIndicator::calculate() {
    const std::vector<double>* price_ptr = nullptr;
    if (column_ == "Close") price_ptr = &df_.close;
    else if (column_ == "Open") price_ptr = &df_.open;
    else if (column_ == "High") price_ptr = &df_.high;
    else if (column_ == "Low") price_ptr = &df_.low;
    else if (column_ == "Volume") price_ptr = &df_.volume;
    else throw std::invalid_argument("Unknown column for RSI calculation: " + column_);
    const std::vector<double>& price = *price_ptr;
    size_t n = price.size();

    std::vector<double> rsi = calculate_rsi(price, window_);

    std::vector<bool> buy_signal(n, false);
    std::vector<bool> sell_signal(n, false);
    for (size_t i = 1; i < n; ++i) {
        // Cross above 30 (buy)
        if (!std::isnan(rsi[i]) && !std::isnan(rsi[i-1]) && rsi[i-1] <= 30.0 && rsi[i] > 30.0)
            buy_signal[i] = true;
        // Cross below 70 (sell)
        if (!std::isnan(rsi[i]) && !std::isnan(rsi[i-1]) && rsi[i-1] >= 70.0 && rsi[i] < 70.0)
            sell_signal[i] = true;
    }

    DataFrame out = df_;
    out.extra_columns[rsi_col_] = rsi;
    out.extra_bool_columns[buy_signal_col_] = buy_signal;
    out.extra_bool_columns[sell_signal_col_] = sell_signal;
    return out;
}

REGISTER_INDICATOR("RSI",RSIIndicator)