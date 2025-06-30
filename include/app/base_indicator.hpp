#pragma once
#include "base_fetcher.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

// Abstract base class for technical indicators.
class Indicator {
public:
    // Required OHLC columns (Volume is optional)
    static const std::vector<std::string>& required_columns() {
        static const std::vector<std::string> cols = {"Open", "High", "Low", "Close"};
        return cols;
    }

    // Constructor: initializes the indicator with a DataFrame (must copy for safety)
    explicit Indicator(const DataFrame& df) : df_(df) {
        validate_ohlcv(df_);
        // signal_orientations is empty by default
    }

    // Virtual destructor for inheritance safety
    virtual ~Indicator() = default;

    // Abstract (pure virtual) method for calculating the indicator.
    // Should be implemented by subclasses.
    virtual DataFrame calculate() = 0;

    // Returns the map from signal names to orientation ("buy", "sell", etc.)
    const std::map<std::string, std::string>& get_signal_orientations() const {
        return signal_orientations_;
    }

    // Returns the DataFrame with calculated indicator columns (after calculate())
    const DataFrame& get_data() const {
        return df_;
    }

protected:
    DataFrame df_; // Copy of input DataFrame to avoid outside modification
    std::map<std::string, std::string> signal_orientations_;

    void validate_ohlcv(const DataFrame& df) const {
        // Check for required columns (by presence of corresponding vectors)
        for (const auto& col : required_columns()) {
            if (!column_exists(df, col)) {
                throw std::invalid_argument("DataFrame is missing required OHLC column: " + col);
            }
        }
        // Check for non-empty
        if (df.size() == 0) {
            throw std::invalid_argument("Input DataFrame cannot be empty.");
        }
        // DateTimeIndex presence is up to you; here just warn if empty
        if (df.datetime_index.empty()) {
            // Could log or throw if required
        }
    }

    bool column_exists(const DataFrame& df, const std::string& col) const {
        // Map column name to actual DataFrame member
        if (col == "Open") return !df.open.empty();
        if (col == "High") return !df.high.empty();
        if (col == "Low") return !df.low.empty();
        if (col == "Close") return !df.close.empty();
        if (col == "Volume") return !df.volume.empty();
        if (col == "Timestamp" || col == "DateTimeIndex") return !df.datetime_index.empty();
        return false;
    }
};
