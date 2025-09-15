#include "utility.hpp"

namespace api {
    engine::OrderType stringToOrderType(const std::string& typeStr) {
        if (typeStr == "BUY") {
            return engine::OrderType::BUY;
        }
        else if (typeStr == "SELL") {
            return engine::OrderType::SELL;
        }
        else {
            throw std::invalid_argument("Invalid order type: " + typeStr);
        }
    }

    engine::OrderMode stringToOrderMode(const std::string& modeStr) {
        if (modeStr == "MARKET") {
            return engine::OrderMode::MARKET;
        }
        else if (modeStr == "LIMIT") {
            return engine::OrderMode::LIMIT;
        }
        else {
            throw std::invalid_argument("Invalid order mode: " + modeStr);
        }
    }

    bool bodyContainsRequiredFields(const crow::json::rvalue& body, const std::vector<std::string>& requiredFields) {
        for (const auto& field : requiredFields) {
            if (!body.has(field)) {
                return false;
            }
        }
        return true;
    }
}