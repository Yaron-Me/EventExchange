#include "Order.hpp"
#include "../utility/uuid.hpp"

namespace exchange {
    Order::Order(User& _parentUser,
                 const boost::uuids::uuid& _ownerId, const OrderType _type,
                 const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                 const std::uint32_t _quantity, const std::uint16_t _price) :
        id{utility::generateUUID()}, ownerId{_ownerId}, type{_type},
        eventId{_eventId}, shareId{_shareId}, quantity{_quantity}, price{_price},
        filledQuantity{0}, fillCosts{0}, parentUser{_parentUser} {}

    OrderType stringToOrderType(const std::string& typeStr) {
        if (typeStr == "BUY") {
            return exchange::OrderType::BUY;
        }
        else if (typeStr == "SELL") {
            return exchange::OrderType::SELL;
        }
        else {
            throw std::invalid_argument("Invalid order type: " + typeStr);
        }
    }

    std::uint32_t Order::leftOverQuantity() const {
        return quantity - filledQuantity;
    }

    std::uint64_t Order::positionValue() const {
        return leftOverQuantity() * price;
    }

    Order::~Order() {
        std::print("{}\n", fillCosts);
        std::print("{}\n", parentUser.getOrderCount());
    }
}