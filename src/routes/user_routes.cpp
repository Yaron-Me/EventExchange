#include "user_routes.hpp"
#include <iostream>

namespace routes {
    void setupUserRoutes(crow::SimpleApp& app) {
        // User registration route
        CROW_ROUTE(app, "/register").methods("POST"_method)
        ([](const crow::request& req) {
            std::cout << "Received POST request to /register" << std::endl;
            std::cout << "Request body: " << req.body << std::endl;
            
            // Parse JSON from request body
            auto body = crow::json::load(req.body);
            if (!body) {
                std::cerr << "Invalid JSON in request body" << std::endl;
                return crow::response(400, "Invalid JSON");
            }
            
            // Extract username and password from JSON
            if (!body.has("username") || !body.has("password")) {
                std::cerr << "Missing username or password in JSON" << std::endl;
                return crow::response(400, "Missing username or password");
            }
            
            std::string username = body["username"].s();
            std::string password = body["password"].s();
            
            std::cout << "Attempting to register user: " << username << std::endl;
            
            if (database::registerUser(username, password)) {
                return crow::response(200, "User registered successfully");
            } else {
                return crow::response(400, "Failed to register user");
            }
        });
    }
}
