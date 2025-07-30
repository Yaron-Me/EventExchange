#pragma once

#include <memory>
#include <deque>

// Forward declaration to avoid circular dependencies
namespace exchange {
    class Order;
}

namespace exchange {
    class OrderQueue {
        public:

        private:
            std::deque<std::shared_ptr<Order>> orderQueue;
    };
}