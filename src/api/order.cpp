#include <limits>

#include "order.hpp"
#include "../exchange/Exchange.hpp"
#include "../utility/uuid.hpp"

namespace api {
    void setupOrderApi(crow::SimpleApp& app, exchange::Exchange& exchange) {
        CROW_ROUTE(app, "/api/orders").methods("POST"_method)
        ([&exchange](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }

            if (!body.has("user_id") || !body.has("type") ||
                !body.has("event_id") || !body.has("share_id") ||
                !body.has("quantity") || !body.has("price")) {
                return crow::response{400, "Missing required fields"};
            }

            try {
                const auto userId{utility::stringToUUID(body["user_id"].s())};
                const auto eventId{utility::stringToUUID(body["event_id"].s())};
                const auto shareId{utility::stringToUUID(body["share_id"].s())};
                const auto type{exchange::stringToOrderType(body["type"].s())};

                if (body["quantity"].t() != crow::json::type::Number ||
                    floor(body["quantity"].d()) != body["quantity"].d() ||
                    body["price"].t() != crow::json::type::Number ||
                    floor(body["price"].d()) != body["price"].d()) {
                    return crow::response{400, "Quantity or price must be an integer"};
                }

                if (body["quantity"].u() > std::numeric_limits<std::uint32_t>::max() ||
                    body["quantity"].u() <= 0 ||
                    body["price"].u() > std::numeric_limits<std::uint16_t>::max() ||
                    body["price"].u() <= 0) {
                    return crow::response{400, "Quantity or price exceeds limits"};
                }

                const std::uint32_t quantity{static_cast<std::uint32_t>(body["quantity"].u())};
                const std::uint16_t price{static_cast<std::uint16_t>(body["price"].u())};

                return exchange.createOrder(userId, eventId, shareId, type, quantity, price);
            }
            catch (const std::exception& e) {
                return crow::response{400, "Invalid UUID or order type"};
            }

            return crow::response{201, "Order created successfully"};
        });
    }
}