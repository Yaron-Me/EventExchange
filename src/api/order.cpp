#include <limits>
#include <print>
#include <iostream>

#include "order.hpp"
#include "../engine/exchange.hpp"
#include "../utility/uuid.hpp"
#include "../engine/order.hpp"
#include "utility.hpp"

namespace api {
    void setupOrderApi(crow::SimpleApp& app, engine::Exchange& exchange) {
        CROW_ROUTE(app, "/api/orders").methods("POST"_method)
        ([&exchange](const crow::request& req) {
            auto body{crow::json::load(req.body)};
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }

            if (!bodyContainsRequiredFields(body, {"user_id", "type", "mode", "event_id", "share_id", "quantity", "price"})) {
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
                    body["price"].u() >= engine::MAX_DENOMINATIONS ||
                    body["price"].u() <= 0) {
                    return crow::response{400, "Quantity or price exceeds limits"};
                }

                const std::uint32_t quantity{static_cast<std::uint32_t>(body["quantity"].u())};
                const std::uint16_t price{static_cast<std::uint16_t>((mode == engine::OrderMode::LIMIT ? body["price"].u() : engine::MAX_DENOMINATIONS - 1))};

                auto [success, message]{exchange.createOrder(userId, eventId, shareId, type, mode, quantity, price)};
                if (success) {
                    return crow::response{201, message};
                } else {
                    return crow::response{400, message};
                }
            }
            catch (const std::exception& e) {
                std::print(std::cerr, "Error creating order: {}\n", e.what());
                return crow::response{400, "Failed to create order."};
            }
        });
    }
}