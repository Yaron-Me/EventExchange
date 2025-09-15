#pragma once

#include <crow.h>

#include "../engine/exchange.hpp"

namespace api {
    void setupOrderApi(crow::SimpleApp& app, engine::Exchange& exchange);
}