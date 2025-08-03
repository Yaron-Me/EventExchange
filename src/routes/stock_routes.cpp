#include "stock_routes.hpp"
#include "../database/exchange.hpp"
#include "../utility/uuid.hpp"

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
                return crow::response{400, "Missing required fields"};
            }
            
            const std::string name{body["name"].s()};
            const std::string description{body["description"].s()};
            const std::string yesShareName{body["yes_share_name"].s()};
            const std::string noShareName{body["no_share_name"].s()};
            
            if (database::createStock(name, description, yesShareName, noShareName)) {
                return crow::response{200, "Stock created successfully"};
            } else {
                return crow::response{400, "Failed to create stock"};
            }
        });

        
        CROW_ROUTE(app, "/stocks").methods("GET"_method)
        ([]() {
            auto stocks = database::getStocks();

            crow::json::wvalue::list response;
            for (const auto& stock : stocks) {
                crow::json::wvalue stockJson;
                stockJson["id"] = utility::uuidToString(stock.id);
                stockJson["name"] = stock.name;
                stockJson["description"] = stock.description;
                stockJson["created_at"] = stock.createdAt;
                stockJson["yes_share"] = crow::json::wvalue{{"id", utility::uuidToString(stock.yesShare.id)},
                                                            {"name", stock.yesShare.name}};
                stockJson["no_share"] = crow::json::wvalue{{"id", utility::uuidToString(stock.noShare.id)},
                                                            {"name", stock.noShare.name}};
                response.push_back(stockJson);
            }
            return crow::response{crow::json::wvalue{response}};
        });
    }
}
