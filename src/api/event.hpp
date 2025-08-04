#pragma once

#include <crow.h>
#include "../exchange/Exchange.hpp"

namespace api {
    void setupEventApi(crow::SimpleApp& app, exchange::Exchange& exchange);
}
