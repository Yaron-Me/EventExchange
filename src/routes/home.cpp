#include "home.hpp"

namespace routes {
    void setupHomeRoute(crow::SimpleApp& app) {
        CROW_ROUTE(app, "/")([] {
            return crow::mustache::load("home.html").render();
        });
    }
}
