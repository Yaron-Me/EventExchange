#pragma once

#include <crow.h>
#include "../database/database.hpp"

namespace routes {
    void setupUserRoutes(crow::SimpleApp& app);
}
