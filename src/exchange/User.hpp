#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <boost/uuid.hpp>

// Forward declaration to avoid circular dependencies
namespace exchange {
    class Order;
}

namespace exchange {
    class User {
        public:
            boost::uuids::uuid id;

            std::int64_t getPositionsValue() const {
                std::int64_t totalValue = 0;
                for (const auto& order : orders) {
                    totalValue += order->positionValue();
                }
                return totalValue;
            }

        private:
            std::vector<std::shared_ptr<Order>> orders;
    };
}
