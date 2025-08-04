#pragma once

#include <crow.h>
#include "../exchange/Exchange.hpp"

namespace api {
    void setupStockApi(crow::SimpleApp& app, exchange::Exchange& exchange);
}
