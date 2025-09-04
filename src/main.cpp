#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>

#include "utility/uuid.hpp"

#include "exchange/Order.hpp"
#include "exchange/Event.hpp"
#include "exchange/Exchange.hpp"

#include "api/api.hpp"
#include "routes/routes.hpp"

#include "database/utility.hpp"
#include "database/exchange.hpp"
#include "database/users.hpp"

int main() {
    exchange::Exchange exchange;
    // if databasefile does not exist, create it
    if (!std::filesystem::exists("proddb.db3")) {
        database::initializeDatabase("../src/database/dblayout.sql", "proddb.db3");

        database::createEvent(exchange, "Will I finish this project?",
                              "An event to test the API", "Yes", "No");
        database::createEvent(exchange, "Who will win the fight?",
                              "An event to test the API", "Fighter A", "Fighter B");
        database::createEvent(exchange, "Example Event", "This is an example event",
                           "Yes Share", "No Share");

        database::registerUser("testuser", "password123");
        database::registerUser("anotheruser", "securepassword");
        database::registerUser("admin", "adminpassword");
    }

    crow::SimpleApp app;
    crow::mustache::set_global_base("../src/templates");

    // Setup all routes
    api::setupApi(app, exchange);
    routes::setupRoutes(app);

    // Start the application
    app.port(18080).run();

    return 0;
}