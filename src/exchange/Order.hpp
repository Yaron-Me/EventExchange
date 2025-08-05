#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <ostream>

#include "../utility/uuid.hpp"

// Forward declarations to avoid circular dependencies
namespace exchange {
    class OrderQueue;
    class User;
}

namespace exchange {
    static const std::uint16_t MAX_DENOMINATIONS = 100;

    enum class OrderType {
        BUY,
        SELL
    };

    OrderType stringToOrderType(const std::string& typeStr);

    std::ostream& operator<<(std::ostream& os, const OrderType& type);

    class Order {
        public:
            const boost::uuids::uuid id;
            const boost::uuids::uuid ownerId;
            const OrderType type;
            const boost::uuids::uuid eventId;
            const boost::uuids::uuid shareId;
            const std::uint32_t quantity;
            const std::uint16_t price;

            Order(OrderQueue& _parentQueue, User& _parentUser,
                  const boost::uuids::uuid& _ownerId, const OrderType _type,
                  const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                  const std::uint32_t _quantity, const std::uint16_t _price);

            std::uint32_t leftOverQuantity() const {
                return quantity - filledQuantity;
            }

            std::uint64_t positionValue() const {
                return leftOverQuantity() * price;
            }

        private:
            std::uint32_t filledQuantity;
            std::uint64_t fillCosts;

            OrderQueue& parentQueue;
            User& parentUser;
    };
}