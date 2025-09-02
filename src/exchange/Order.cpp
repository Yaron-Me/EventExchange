#include "Order.hpp"
#include "../utility/uuid.hpp"

namespace exchange {
    Order::Order(const boost::uuids::uuid& _ownerId, const OrderType _type, const OrderMode _mode,
                 const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                 const std::uint32_t _quantity, const std::uint16_t _price) :
        id{utility::generateUUID()}, ownerId{_ownerId}, type{_type}, mode{_mode},
        eventId{_eventId}, shareId{_shareId}, quantity{_quantity}, price{_price},
        filledQuantity{0}, fillCosts{0} {}

    std::uint32_t Order::leftOverQuantity() const {
        return quantity - filledQuantity;
    }

    std::uint64_t Order::positionValue() const {
        return leftOverQuantity() * price;
    }

    Order::~Order() {
        std::print("{}\n", fillCosts);
    }
}