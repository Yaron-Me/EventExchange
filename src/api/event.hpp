#pragma once

#include <crow.h>
#include "../engine/exchange.hpp"

namespace api {
    void setupEventApi(crow::SimpleApp& app, engine::Exchange& exchange);
}
