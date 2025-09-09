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
            void addOrder(const std::shared_ptr<Order>& order);
            void cancelOrder(const std::shared_ptr<Order>& order);
            std::uint64_t getTotalQuantity() const;
            std::size_t getOrderCount() const;
            std::uint64_t fill(std::uint64_t fillQuantity, std::uint16_t fillPrice);

        private:
            std::deque<std::shared_ptr<Order>> orderQueue;
            std::uint64_t totalQuantity{0};
    };
}