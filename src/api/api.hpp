#pragma once

#include <crow.h>
#include "user.hpp"
#include "stock.hpp"
#include "order.hpp"

namespace api {
    void setupApi(crow::SimpleApp& app, exchange::Exchange& exchange) {
        setupUserApi(app);
        setupStockApi(app);
        setupOrderApi(app, exchange);
    }
}