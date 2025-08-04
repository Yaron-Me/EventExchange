#pragma once

#include <crow.h>

#include "../exchange/Exchange.hpp"

namespace api {
    void setupOrderApi(crow::SimpleApp& app, exchange::Exchange& exchange);
}