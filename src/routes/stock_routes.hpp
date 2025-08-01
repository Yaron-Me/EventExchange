#pragma once

#include <crow.h>
#include "../database/database.hpp"

namespace routes {
    void setupStockRoutes(crow::SimpleApp& app);
}
