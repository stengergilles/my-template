#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include "base_indicator.hpp"

// Factory for creating Indicator subclasses by string name
class IndicatorFactory {
public:
    using Creator = std::function<std::unique_ptr<Indicator>(const DataFrame&)>;

    // Registers a creator with a string key (indicator name)
    static void register_creator(const std::string& name, Creator creator) {
        get_registry()[name] = std::move(creator);
    }

    // Creates an Indicator of the given type (by name)
    static std::unique_ptr<Indicator> create(const std::string& name, const DataFrame& df) {
        auto it = get_registry().find(name);
        if (it != get_registry().end()) {
            return (it->second)(df);
        } else {
            throw std::invalid_argument("Unknown indicator type: " + name);
        }
    }

    // Returns all registered indicator names
    static std::vector<std::string> registered_names() {
        std::vector<std::string> names;
        for (const auto& kv : get_registry()) names.push_back(kv.first);
        return names;
    }

private:
    static std::map<std::string, Creator>& get_registry() {
        static std::map<std::string, Creator> registry;
        return registry;
    }
};

// Helper macro for registering an Indicator subclass
#define REGISTER_INDICATOR(name, class_type) \
    namespace { \
        struct Register##class_type { \
            Register##class_type() { \
                IndicatorFactory::register_creator(name, [](const DataFrame& df) { \
                    return std::make_unique<class_type>(df); \
                }); \
            } \
        }; \
        static Register##class_type reg_##class_type; \
    }
