#pragma once
#include "base_indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>

class MACDIndicator : public Indicator {
public:
    MACDIndicator(const DataFrame& df,
                  int fast_period = 12,
                  int slow_period = 26,
                  int signal_period = 9,
                  const std::string& column = "Close");
    DataFrame calculate();

private:
    int fast_period_;
    int slow_period_;
    int signal_period_;
    std::string column_;
    std::string macd_col_;
    std::string signal_col_;
    std::string hist_col_;
    std::string buy_signal_col_;
    std::string sell_signal_col_;

    std::vector<double> calculate_ema(const std::vector<double>& data, int period);
};
