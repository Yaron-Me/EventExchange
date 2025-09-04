#include "Order.hpp"
#include "../utility/uuid.hpp"

namespace exchange {
    Order::Order(const boost::uuids::uuid& _ownerId, const OrderType _type, const OrderMode _mode,
                 const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                 const std::uint32_t _quantity, const std::uint16_t _price) :
        id{utility::generateUUID()}, ownerId{_ownerId}, type{_type}, mode{_mode},
        eventId{_eventId}, shareId{_shareId}, quantity{_quantity}, price{_price},
        filledQuantity{0}, transactedMoney{0} {}

    std::uint32_t Order::leftoverQuantitiy() const {
        return quantity - filledQuantity;
    }

    std::uint64_t Order::positionValue() const {
        return leftoverQuantitiy() * price;
    }

    std::uint64_t Order::totalTransactedMoney() const {
        return transactedMoney;
    }

    std::uint32_t Order::fill(std::uint32_t amount, std::uint16_t fillPrice) {
        if ((type == OrderType::BUY && fillPrice > price) ||
            (type == OrderType::SELL && fillPrice < price)) return 0;
        
        const auto leftToFill = leftoverQuantitiy();

        const auto amountToFill = amount <= leftToFill ? amount : leftToFill;

        filledQuantity += amountToFill;
        transactedMoney += amountToFill * fillPrice;

        

        return amountToFill;
    }

    Order::~Order() {
        std::print("{}\n", transactedMoney);
    }
}