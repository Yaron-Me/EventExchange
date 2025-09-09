#pragma once

#include <crow.h>

#include "../exchange/exchange.hpp"

namespace api {
    void setupOrderApi(crow::SimpleApp& app, exchange::Exchange& exchange);
}