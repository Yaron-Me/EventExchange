#pragma once

#include "../engine/order.hpp"

#include <vector>
#include <string>
#include <crow.h>

namespace api {
    engine::OrderType stringToOrderType(const std::string& typeStr);
    engine::OrderMode stringToOrderMode(const std::string& modeStr);
    bool bodyContainsRequiredFields(const crow::json::rvalue& body, const std::vector<std::string>& requiredFields);
}