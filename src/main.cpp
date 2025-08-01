#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>

#include "utility/uuid.hpp"
#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"
#include "routes/user_routes.hpp"
#include "routes/stock_routes.hpp"

int main() {
    // database::initializeDatabase("../src/database/dblayout.txt");

    crow::SimpleApp app;

    // Setup all routes
    routes::setupUserRoutes(app);
    routes::setupStockRoutes(app);

    std::cout << "Starting server on port 18080..." << std::endl;

    app.port(18080).run();

    return 0;
}