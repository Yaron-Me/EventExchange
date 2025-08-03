#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>

#include "utility/uuid.hpp"

#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"

#include "routes/user_routes.hpp"
#include "routes/stock_routes.hpp"

#include "database/utility.hpp"

int main() {
    // if databasefile does not exist, create it
    if (!std::filesystem::exists("exchange.db3")) {
        database::initializeDatabase("../src/database/dblayout.sql");
    }

    crow::SimpleApp app;
    // exchange::Exchange exchange;

    // Setup all routes
    routes::setupUserRoutes(app);
    routes::setupStockRoutes(app);

    app.port(18080).run();

    return 0;
}