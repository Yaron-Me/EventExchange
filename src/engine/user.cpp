#include <map>

#include "user.hpp"
#include "order.hpp"

namespace engine {
    std::int64_t User::getTiedUpBalance() {
        std::int64_t totalValue{0};
        std::erase_if(orders, [&totalValue](const std::weak_ptr<Order>& wp) {
            if (auto order = wp.lock()) {
                if (order->type == OrderType::BUY) {
                    totalValue += order->positionValue();
                }
                return false;
            }
            return true;
        });
        return totalValue;
    }

    std::map<boost::uuids::uuid, std::uint32_t> User::getSellOrderShareCounts() {
        std::map<boost::uuids::uuid, std::uint32_t> shares;
        std::erase_if(orders, [&shares](const std::weak_ptr<Order>& wp) {
            if (auto order = wp.lock()) {
                if (order->type == OrderType::SELL) {
                    shares[order->shareId] += order->leftoverQuantitiy();
                }
                return false;
            }
            return true;
        });
        return shares;
    }

    std::size_t User::getOrderCount() const {
        return orders.size();
    }

    void User::addOrder(std::weak_ptr<Order> order) {
        orders.push_back(order);
    }
}
