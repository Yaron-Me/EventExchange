#pragma once

#include <crow.h>
#include "../exchange/Exchange.hpp"

namespace routes {
    void setupStockRoutes(crow::SimpleApp& app);
}
