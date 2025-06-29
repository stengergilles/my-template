#pragma once
#include "base_indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

class BreakoutIndicator : public Indicator {
public:
    BreakoutIndicator(const DataFrame& df, int window = 20,
                      const std::string& high_col = "High",
                      const std::string& low_col = "Low",
                      const std::string& close_col = "Close");
    DataFrame calculate();

private:
    int window_;
    std::string high_col_;
    std::string low_col_;
    std::string close_col_;
    std::string bullish_signal_col_;
    std::string bearish_signal_col_;
};
