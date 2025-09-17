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
        std::uint16_t queuePrice{0};
        
        while ((order->leftoverQuantitiy() - totalFilled) > 0 && getTotalQuantity() > 0) {
            auto& currentOrder{orderQueue.front()};
            queuePrice = currentOrder->price; // All orders in queue have same price

            const auto filled{currentOrder->fill(order->leftoverQuantitiy() - totalFilled, queuePrice)};
            
            totalQuantity -= filled;
            totalFilled += filled;

            if (currentOrder->leftoverQuantitiy() == 0) {
                orderQueue.pop_front();
            }
        }
        
        // Fill the incoming order once with the total amount at the queue's price
        if (totalFilled > 0) {
            order->fill(totalFilled, queuePrice);
        }
        
        return totalFilled;
    }
}