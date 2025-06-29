#pragma once
#include "app/base_indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>

class VolumeSpikeIndicator : public Indicator {
public:
    VolumeSpikeIndicator(const DataFrame& df,
                         int window = 20,
                         double threshold = 2.0,
                         const std::string& column = "Volume");
    DataFrame calculate();

private:
    int window_;
    double threshold_;
    std::string column_;
    std::string spike_col_;
    std::string buy_signal_col_;
    std::string sell_signal_col_;

    std::vector<double> calculate_sma(const std::vector<double>& data, int window);
};
