#pragma once
#include "app/base_indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>

class RSIIndicator : public Indicator {
public:
    RSIIndicator(const DataFrame& df,
                 int window = 14,
                 const std::string& column = "Close");
    DataFrame calculate();

private:
    int window_;
    std::string column_;
    std::string rsi_col_;
    std::string buy_signal_col_;
    std::string sell_signal_col_;

    std::vector<double> calculate_rsi(const std::vector<double>& data, int window);
};
