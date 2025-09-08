#include "utility.hpp"

namespace api {
    exchange::OrderType stringToOrderType(const std::string& typeStr) {
        if (typeStr == "BUY") {
            return exchange::OrderType::BUY;
        }
        else if (typeStr == "SELL") {
            return exchange::OrderType::SELL;
        }
        else {
            throw std::invalid_argument("Invalid order type: " + typeStr);
        }
    }

    exchange::OrderMode stringToOrderMode(const std::string& modeStr) {
        if (modeStr == "MARKET") {
            return exchange::OrderMode::MARKET;
        }
        else if (modeStr == "LIMIT") {
            return exchange::OrderMode::LIMIT;
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