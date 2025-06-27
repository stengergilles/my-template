#include "breakout_indicator.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

// Utility: get reference to a column by name
static const std::vector<double>& get_column(const DataFrame& df, const std::string& name) {
    if (name == "High") return df.high;
    if (name == "Low") return df.low;
    if (name == "Close") return df.close;
    if (name == "Open") return df.open;
    if (name == "Volume") return df.volume;
    auto it = df.extra_columns.find(name);
    if (it != df.extra_columns.end()) return it->second;
    throw std::invalid_argument("Column not found: " + name);
}

BreakoutIndicator::BreakoutIndicator(const DataFrame& df, int window,
                                     const std::string& high_col,
                                     const std::string& low_col,
                                     const std::string& close_col)
    : Indicator(df), window_(window),
      high_col_(high_col), low_col_(low_col), close_col_(close_col)
{
    // No need to check .columns, just check the actual members
    size_t n = get_column(df_, close_col_).size();
    if (n < static_cast<size_t>(window_) + 1) {
        throw std::invalid_argument("Insufficient data for BreakoutIndicator (window: " +
            std::to_string(window_) + "): " +
            std::to_string(n) +
            " rows provided, requires at least " + std::to_string(window_ + 1) + " rows.");
    }
    bullish_signal_col_ = "Breakout_Bullish_Signal_" + std::to_string(window_);
    bearish_signal_col_ = "Breakout_Bearish_Signal_" + std::to_string(window_);

    signal_orientations_[bullish_signal_col_] = "buy";
    signal_orientations_[bearish_signal_col_] = "sell";
}

DataFrame BreakoutIndicator::calculate() {
    const auto& high = get_column(df_, high_col_);
    const auto& low = get_column(df_, low_col_);
    const auto& close = get_column(df_, close_col_);
    size_t n = close.size();

    std::vector<bool> bullish_signal(n, false);
    std::vector<bool> bearish_signal(n, false);

    for (size_t i = window_; i < n; ++i) {
        double recent_high = *std::max_element(high.begin() + (i - window_), high.begin() + i);
        double recent_low = *std::min_element(low.begin() + (i - window_), low.begin() + i);
        if (close[i] > recent_high)
            bullish_signal[i] = true;
        if (close[i] < recent_low)
            bearish_signal[i] = true;
    }

    DataFrame out = df_;
    out.extra_bool_columns[bullish_signal_col_] = bullish_signal;
    out.extra_bool_columns[bearish_signal_col_] = bearish_signal;
    return out;
}
