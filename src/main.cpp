#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>

#include "utility/UUID.hpp"
#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"
#include "database/database.hpp"
#include "routes/user_routes.hpp"
#include "routes/stock_routes.hpp"

int main() {
    // database::initializeDatabase("../src/database/dblayout.txt");

    // database::registerUser("testuser", "password123");

    // database::createStock("Will I finish this project?",
    //                       "A stock to track the progress of my project",
    //                       "Yes",
    //                       "No");

    // database::createStock("Will it rain tomorrow?", 
    //                       "Prediction market for weather", 
    //                       "Yes",
    //                       "No");

    crow::SimpleApp app;

    // Setup all routes
    routes::setupUserRoutes(app);
    routes::setupStockRoutes(app);

    std::cout << "Starting server on port 18080..." << std::endl;

    app.port(18080).run();

    return 0;
}