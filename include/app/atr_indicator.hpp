#pragma once
#include "base_indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

// Helper for rolling mean/median
namespace rolling {

inline std::vector<double> mean(const std::vector<double>& data, int window) {
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

inline std::vector<double> median(const std::vector<double>& data, int window, int min_periods) {
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

} // namespace rolling

class ATRIndicator : public Indicator {
public:
    ATRIndicator(const DataFrame& df, int window = 14);
    DataFrame calculate();

private:
    int window_;
    std::string atr_col_name_;
    std::string low_atr_signal_col_;
    std::string high_atr_signal_col_;
};
