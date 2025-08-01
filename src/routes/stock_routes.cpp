#include "stock_routes.hpp"
#include <iostream>

namespace routes {
    void setupStockRoutes(crow::SimpleApp& app) {
        // Create stock route
        CROW_ROUTE(app, "/stocks").methods("POST"_method)
        ([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }
            
            if (!body.has("name") || !body.has("description") || 
                !body.has("yes_share_name") || !body.has("no_share_name")) {
                return crow::response(400, "Missing required fields");
            }
            
            std::string name = body["name"].s();
            std::string description = body["description"].s();
            std::string yesShareName = body["yes_share_name"].s();
            std::string noShareName = body["no_share_name"].s();
            
            if (database::createStock(name, description, yesShareName, noShareName)) {
                return crow::response(200, "Stock created successfully");
            } else {
                return crow::response(400, "Failed to create stock");
            }
        });
    }
}
