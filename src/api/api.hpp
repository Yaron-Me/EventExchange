#pragma once

#include <crow.h>
#include "user.hpp"
#include "event.hpp"
#include "order.hpp"

namespace api {
    void setupApi(crow::SimpleApp& app, exchange::Exchange& exchange) {
        setupUserApi(app);
        setupEventApi(app, exchange);
        setupOrderApi(app, exchange);
    }
}