#include "app/ma_indicator.hpp"
#include "app/indicator_factory.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

MAIndicator::MAIndicator(const DataFrame& df, int window, const std::string& ma_type, const std::string& column)
    : Indicator(df), window_(window), ma_type_(ma_type), column_(column)
{
    size_t n = 0;
    if (column_ == "Close") n = df_.close.size();
    else if (column_ == "Open") n = df_.open.size();
    else if (column_ == "High") n = df_.high.size();
    else if (column_ == "Low") n = df_.low.size();
    else if (column_ == "Volume") n = df_.volume.size();
    else throw std::invalid_argument("Unknown column for MA calculation: " + column_);

    if (n < static_cast<size_t>(window_)) {
        throw std::invalid_argument("Insufficient data for MAIndicator (window: " +
            std::to_string(window_) + "): " + std::to_string(n) +
            " rows provided, requires at least " + std::to_string(window_) + " rows.");
    }

    std::string ma_type_uc = ma_type_;
    std::transform(ma_type_uc.begin(), ma_type_uc.end(), ma_type_uc.begin(), ::toupper);
    ma_col_name_ = ma_type_uc + "_" + std::to_string(window_) + "_" + column_;
    buy_signal_col_ = ma_col_name_ + "_Cross_Above";
    sell_signal_col_ = ma_col_name_ + "_Cross_Below";

    signal_orientations_[buy_signal_col_] = "buy";
    signal_orientations_[sell_signal_col_] = "sell";
}

std::vector<double> MAIndicator::calculate_sma(const std::vector<double>& data, int window) {
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

std::vector<double> MAIndicator::calculate_ema(const std::vector<double>& data, int window) {
    std::vector<double> result(data.size(), std::nan(""));
    if (window <= 0 || data.size() < static_cast<size_t>(window)) return result;
    double alpha = 2.0 / (window + 1.0);
    double ema = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i == window - 1) {
            double sum = 0.0;
            for (int j = 0; j < window; ++j) sum += data[j];
            ema = sum / window;
            result[i] = ema;
        } else if (i >= static_cast<size_t>(window)) {
            ema = alpha * data[i] + (1.0 - alpha) * ema;
            result[i] = ema;
        }
    }
    return result;
}

DataFrame MAIndicator::calculate() {
    const std::vector<double>* price_ptr = nullptr;
    if (column_ == "Close") price_ptr = &df_.close;
    else if (column_ == "Open") price_ptr = &df_.open;
    else if (column_ == "High") price_ptr = &df_.high;
    else if (column_ == "Low") price_ptr = &df_.low;
    else if (column_ == "Volume") price_ptr = &df_.volume;
    else throw std::invalid_argument("Unknown column for MA calculation: " + column_);
    const std::vector<double>& price = *price_ptr;
    size_t n = price.size();

    std::vector<double> ma_series;
    if (ma_type_ == "sma") {
        ma_series = calculate_sma(price, window_);
    } else if (ma_type_ == "ema") {
        ma_series = calculate_ema(price, window_);
    } else {
        throw std::invalid_argument("Unknown ma_type: " + ma_type_);
    }

    std::vector<bool> buy_signal(n, false);
    std::vector<bool> sell_signal(n, false);

    for (size_t i = 1; i < n; ++i) {
        if (!std::isnan(ma_series[i]) && !std::isnan(ma_series[i-1])) {
            if (price[i] > ma_series[i] && price[i-1] <= ma_series[i-1])
                buy_signal[i] = true;
            if (price[i] < ma_series[i] && price[i-1] >= ma_series[i-1])
                sell_signal[i] = true;
        }
    }

    DataFrame out = df_;
    out.extra_columns[ma_col_name_] = ma_series;
    out.extra_bool_columns[buy_signal_col_] = buy_signal;
    out.extra_bool_columns[sell_signal_col_] = sell_signal;
    return out;
}

REGISTER_INDICATOR("MA",MAIndicator)
