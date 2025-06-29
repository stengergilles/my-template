#pragma once
#include "base_indicator.hpp"
#include <string>

// Bollinger Bands Indicator
class BollingerBandsIndicator : public Indicator {
public:
    BollingerBandsIndicator(const DataFrame& df, int window = 20, double num_std = 2.0);

    DataFrame calculate() override;

private:
    int window_;
    double num_std_;
    std::string bb_middle_col_;
    std::string bb_upper_col_;
    std::string bb_lower_col_;
    std::string signal_buy_col_;
    std::string signal_sell_col_;
};
