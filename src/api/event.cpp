#include "event.hpp"
#include "../database/exchange.hpp"
#include "../utility/uuid.hpp"
#include "utility.hpp"

namespace api {
    void setupEventApi(crow::SimpleApp& app, engine::Exchange& exchange) {
        // Create event route
        CROW_ROUTE(app, "/api/events").methods("POST"_method)
        ([&exchange](const crow::request& req) {
            auto body{crow::json::load(req.body)};
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }
            
            if (!bodyContainsRequiredFields(body, {"name", "description", "yes_share_name", "no_share_name"})) {
                return crow::response{400, "Missing required fields"};
            }
            
            const std::string name{body["name"].s()};
            const std::string description{body["description"].s()};
            const std::string yesShareName{body["yes_share_name"].s()};
            const std::string noShareName{body["no_share_name"].s()};
            
            auto [success, eventData]{database::createEvent(name, description, yesShareName, noShareName)};

            if (success) {
                exchange.createEvent(eventData.id, eventData.yesShare.id, eventData.noShare.id);
                return crow::response{200, "Event created successfully"};
            } else {
                return crow::response{400, "Failed to create event"};
            }
        });

        
        CROW_ROUTE(app, "/api/events").methods("GET"_method)
        ([]() {
            auto events{database::getEvents()};

            crow::json::wvalue::list response;
            for (const auto& event : events) {
                crow::json::wvalue eventJson;
                eventJson["id"] = utility::uuidToString(event.id);
                eventJson["name"] = event.name;
                eventJson["description"] = event.description;
                eventJson["created_at"] = event.createdAt;
                eventJson["yes_share"] = crow::json::wvalue{{"id", utility::uuidToString(event.yesShare.id)},
                                                            {"name", event.yesShare.name}};
                eventJson["no_share"] = crow::json::wvalue{{"id", utility::uuidToString(event.noShare.id)},
                                                            {"name", event.noShare.name}};
                response.push_back(eventJson);
            }
            return crow::response{crow::json::wvalue{response}};
        });
    }
}
