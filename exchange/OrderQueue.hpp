#pragma once

#include <memory>
#include <deque>

#include "exchange/Order.hpp"

namespace exchange {
    class OrderQueue {
        public:

        private:
            std::deque<std::shared_ptr<exchange::Order>> orderQueue;
    };
}