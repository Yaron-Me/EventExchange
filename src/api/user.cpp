#include <print>
#include <iostream>

#include "user.hpp"
#include "../database/users.hpp"

namespace api {
    void setupUserApi(crow::SimpleApp& app) {
        CROW_ROUTE(app, "/api/register").methods("POST"_method)
        ([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                std::print(std::cerr, "Invalid JSON in request body\n");
                return crow::response{400, "Invalid JSON"};
            }
            
            if (!body.has("username") || !body.has("password")) {
                std::print(std::cerr, "Missing username or password in JSON\n");
                return crow::response{400, "Missing username or password"};
            }
            
            const std::string username{body["username"].s()};
            const std::string password{body["password"].s()};
        
            if (database::registerUser(username, password)) {
                return crow::response{200, "User registered successfully"};
            } else {
                return crow::response{400, "Failed to register user"};
            }
        });
    }
}
