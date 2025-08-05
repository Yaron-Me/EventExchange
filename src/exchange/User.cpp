#include <map>

#include "User.hpp"
#include "Order.hpp"

namespace exchange {
    std::int64_t User::getTiedUpBalance() const {
        std::int64_t totalValue = 0;
        for (const auto& order : orders) {
            totalValue += order->positionValue();
        }
        return totalValue;
    }

    std::map<boost::uuids::uuid, std::uint32_t> User::getSellOrderShareCounts() const {
        std::map<boost::uuids::uuid, std::uint32_t> shares;
        for (const auto& order : orders) {
            if (order->type == OrderType::SELL) {
                shares[order->shareId] += order->leftOverQuantity();
            }
        }
        return shares;
    }
}
