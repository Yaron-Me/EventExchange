#pragma once

#include <crow.h>
#include "../exchange/exchange.hpp"

namespace api {
    void setupEventApi(crow::SimpleApp& app, exchange::Exchange& exchange);
}
