#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>

#include "utility/uuid.hpp"

#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"

#include "api/api.hpp"
#include "routes/routes.hpp"

#include "database/utility.hpp"

int main() {
    // if databasefile does not exist, create it
    if (!std::filesystem::exists("exchange.db3")) {
        database::initializeDatabase("../src/database/dblayout.sql");
    }

    crow::SimpleApp app;
    crow::mustache::set_global_base("/home/soul/EventExchange/src/templates");
    // exchange::Exchange exchange;

    // Setup all routes
    api::setupApi(app);
    routes::setupRoutes(app);

    // Start the application
    app.port(18080).run();

    return 0;
}