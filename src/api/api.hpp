#include <crow.h>
#include "user.hpp"
#include "stock.hpp"

namespace api {
    void setupApi(crow::SimpleApp& app) {
        setupUserApi(app);
        setupStockApi(app);
    }
}