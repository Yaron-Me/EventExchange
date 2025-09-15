#include "orderqueue.hpp"
#include "order.hpp"

namespace engine {
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

    std::uint64_t OrderQueue::fillOrder(const std::shared_ptr<Order>& order) {

        std::uint64_t totalFilled{0};
        while (order->leftoverQuantitiy() > 0 && getTotalQuantity() > 0) {
            auto& currentOrder{orderQueue.front()};
            const auto price{currentOrder->price};

            const auto filled{currentOrder->fill(order->leftoverQuantitiy(), price)};
            order->fill(filled, price);

            totalQuantity -= filled;
            totalFilled += filled;

            if (currentOrder->leftoverQuantitiy() == 0) {
                orderQueue.pop_front();
            }
        }
        return totalFilled;
    }
}