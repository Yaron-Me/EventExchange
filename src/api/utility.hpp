#pragma once

#include "../exchange/Order.hpp"

#include <vector>
#include <string>
#include <crow.h>

namespace api {
    exchange::OrderType stringToOrderType(const std::string& typeStr);
    exchange::OrderMode stringToOrderMode(const std::string& modeStr);
    bool bodyContainsRequiredFields(const crow::json::rvalue& body, const std::vector<std::string>& requiredFields);
}