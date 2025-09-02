#include <limits>
#include <print>
#include <iostream>

#include "order.hpp"
#include "../exchange/Exchange.hpp"
#include "../utility/uuid.hpp"
#include "../exchange/Order.hpp"

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

namespace api {
    void setupOrderApi(crow::SimpleApp& app, exchange::Exchange& exchange) {
        CROW_ROUTE(app, "/api/orders").methods("POST"_method)
        ([&exchange](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }

            if (!body.has("user_id") || !body.has("type") || !body.has("mode") ||
                !body.has("event_id") || !body.has("share_id") ||
                !body.has("quantity") || !body.has("price")) {
                return crow::response{400, "Missing required fields"};
            }

            try {
                const auto userId{utility::stringToUUID(body["user_id"].s())};
                const auto eventId{utility::stringToUUID(body["event_id"].s())};
                const auto shareId{utility::stringToUUID(body["share_id"].s())};
                const auto type{stringToOrderType(body["type"].s())};
                const auto mode{stringToOrderMode(body["mode"].s())};

                if (body["quantity"].t() != crow::json::type::Number ||
                    floor(body["quantity"].d()) != body["quantity"].d() ||
                    body["price"].t() != crow::json::type::Number ||
                    floor(body["price"].d()) != body["price"].d()) {
                    return crow::response{400, "Quantity or price must be an integer"};
                }

                if (body["quantity"].u() > std::numeric_limits<std::uint32_t>::max() ||
                    body["quantity"].u() <= 0 ||
                    body["price"].u() > exchange::MAX_DENOMINATIONS ||
                    body["price"].u() <= 0) {
                    return crow::response{400, "Quantity or price exceeds limits"};
                }

                const std::uint32_t quantity{static_cast<std::uint32_t>(body["quantity"].u())};
                const std::uint16_t price{static_cast<std::uint16_t>(body["price"].u())};

                return exchange.createOrder(userId, eventId, shareId, type, mode, quantity, price);
            }
            catch (const std::exception& e) {
                std::print(std::cerr, "Error creating order: {}\n", e.what());
                return crow::response{400, "Failed to create order."};
            }

            return crow::response{201, "Order created successfully"};
        });
    }
}