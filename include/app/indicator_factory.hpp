#pragma once

#include "base_indicator.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>

#include "base_fetcher.hpp"

#define REGISTER_INDICATOR(name, class_name) \
namespace { \
    bool registered_##class_name = IndicatorFactory::register_indicator(name, \
        [](const DataFrame& df) { return std::make_unique<class_name>(df); }); \
}

class IndicatorFactory {
public:
    using IndicatorCreator = std::function<std::unique_ptr<Indicator>(const DataFrame&)>;

    static bool register_indicator(const std::string& name, IndicatorCreator creator);
    static std::unique_ptr<Indicator> create_indicator(const std::string& name, const DataFrame& df);
    static std::vector<std::string> get_available_indicators();

private:
    static std::map<std::string, IndicatorCreator>& get_creators();
};
