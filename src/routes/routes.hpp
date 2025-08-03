#include <crow.h>
#include "home.hpp"

namespace routes {
    void setupRoutes(crow::SimpleApp& app) {
        routes::setupHomeRoute(app);
    }
}