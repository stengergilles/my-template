#pragma once
#include "base_indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

class MAIndicator : public Indicator {
public:
    MAIndicator(const DataFrame& df, int window = 20, 
                const std::string& ma_type = "sma", 
                const std::string& column = "Close");
    DataFrame calculate();

private:
    int window_;
    std::string ma_type_; // "sma" or "ema"
    std::string column_;
    std::string ma_col_name_;
    std::string buy_signal_col_;
    std::string sell_signal_col_;

    std::vector<double> calculate_sma(const std::vector<double>& data, int window);
    std::vector<double> calculate_ema(const std::vector<double>& data, int window);
};
