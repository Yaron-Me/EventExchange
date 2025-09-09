#include "orderqueue.hpp"
#include "order.hpp"

namespace exchange {
    void OrderQueue::addOrder(const std::shared_ptr<Order>& order) {
        orderQueue.push_back(order);
        totalQuantity += order->leftoverQuantitiy();
    }

    void OrderQueue::cancelOrder(const std::shared_ptr<Order>& order) {
        auto it{std::ranges::find(orderQueue, order)};
        if (it != orderQueue.end()) {
            totalQuantity -= (*it)->leftoverQuantitiy();
            orderQueue.erase(it);
        }
    }

    std::uint64_t OrderQueue::getTotalQuantity() const {
        return totalQuantity;
    }

    std::size_t OrderQueue::getOrderCount() const {
        return orderQueue.size();
    }

    std::uint64_t OrderQueue::fill(std::uint64_t fillQuantity, std::uint16_t fillPrice) {
        auto leftToFill{fillQuantity};
        while (leftToFill > 0 && getTotalQuantity() > 0) {
            auto& currentOrder{orderQueue.front()};
            const auto filled{currentOrder->fill(leftToFill, fillPrice)};
            leftToFill -= filled;
            totalQuantity -= filled;

            if (currentOrder->leftoverQuantitiy() == 0) {
                orderQueue.pop_front();
            }
        }

        return fillQuantity - leftToFill;
    }
}