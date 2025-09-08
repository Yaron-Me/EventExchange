#include "Order.hpp"
#include "../utility/uuid.hpp"
#include "../database/users.hpp"

namespace exchange {
    Order::Order(const boost::uuids::uuid& _userId, const OrderType _type, const OrderMode _mode,
                 const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                 const std::uint64_t _quantity, const std::uint16_t _price) :
        id{utility::generateUUID()}, userId{_userId}, type{_type}, mode{_mode},
        eventId{_eventId}, shareId{_shareId}, quantity{_quantity}, price{_price},
        filledQuantity{0}, transactedValue{0} {}

    std::uint64_t Order::leftoverQuantitiy() const {
        return quantity - filledQuantity;
    }

    std::uint64_t Order::positionValue() const {
        return leftoverQuantitiy() * price;
    }

    std::uint64_t Order::totalTransactedValue() const {
        return transactedValue;
    }

    std::uint64_t Order::fill(std::uint64_t quantityToFill, std::uint16_t fillPrice) {
        if ((type == OrderType::BUY && fillPrice > price) ||
            (type == OrderType::SELL && fillPrice < price)) return 0;
        
        const auto leftToFill = leftoverQuantitiy();

        const auto amountToFill = quantityToFill <= leftToFill ? quantityToFill : leftToFill;

        filledQuantity += amountToFill;
        transactedValue += amountToFill * fillPrice;

        // Create signed amount for database operations (positive for buy, negative for sell)
        const std::int64_t transactionValue = type == OrderType::BUY ? 
            static_cast<std::int64_t>(amountToFill) : 
            -static_cast<std::int64_t>(amountToFill);

        const std::int64_t moneyChange = type == OrderType::BUY ? 
            -static_cast<std::int64_t>(amountToFill * fillPrice) : 
            static_cast<std::int64_t>(amountToFill * fillPrice);

        database::createTransaction(userId, shareId, transactionValue, fillPrice);
        database::updateUserHoldings(userId, shareId, transactionValue);
        database::updateUserBalance(userId, moneyChange);
        
        return amountToFill;
    }

    Order::~Order() {
        
    }
}