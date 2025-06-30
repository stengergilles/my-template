#include "app/indicator_factory.hpp"
#include "app/base_fetcher.hpp" // For DataFrame
#include <functional>
#include <map>

std::map<std::string, IndicatorFactory::IndicatorCreator>& IndicatorFactory::get_creators() {
    static std::map<std::string, IndicatorCreator> creators;
    return creators;
}

bool IndicatorFactory::register_indicator(const std::string& name, IndicatorCreator creator) {
    get_creators()[name] = creator;
    return true;
}

std::unique_ptr<Indicator> IndicatorFactory::create_indicator(const std::string& name, const DataFrame& df) {
    auto it = get_creators().find(name);
    if (it != get_creators().end()) {
        return it->second(df);
    }
    return nullptr;
}

std::vector<std::string> IndicatorFactory::get_available_indicators() {
    std::vector<std::string> names;
    for (const auto& pair : get_creators()) {
        names.push_back(pair.first);
    }
    return names;
}
