#include "order.hpp"
#include "../utility/uuid.hpp"
#include "../database/user.hpp"
#include "../database/event.hpp"

namespace engine {
    Order::Order(const boost::uuids::uuid& _userId, const OrderType _type, const OrderMode _mode,
                 const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                 const std::uint64_t _quantity, const std::uint16_t _price) :
        id{utility::generateUUID()}, userId{_userId}, type{_type}, mode{_mode},
        eventId{_eventId}, shareId{_shareId}, quantity{_quantity}, price{_price} {}

    std::uint64_t Order::leftoverQuantitiy() const {
        return quantity - filledQuantity;
    }

    std::uint64_t Order::positionValue() const {
        return leftoverQuantitiy() * price;
    }

    std::uint64_t Order::totalTransactedValue() const {
        return transactedValue;
    }

    std::uint64_t Order::fill(std::uint64_t fillQuantity, std::uint16_t fillPrice) {
        if ((type == OrderType::BUY && fillPrice > price) ||
            (type == OrderType::SELL && fillPrice < price)) return 0;
        
        const auto leftToFill{leftoverQuantitiy()};

        const auto quantityToFill{fillQuantity <= leftToFill ? fillQuantity : leftToFill};

        filledQuantity += quantityToFill;
        transactedValue += quantityToFill * fillPrice;

        const std::int64_t transactionQuantity{type == OrderType::BUY ? 
            static_cast<std::int64_t>(quantityToFill) : 
            -static_cast<std::int64_t>(quantityToFill)};

        const std::int64_t transactionValue{type == OrderType::BUY ? 
            -static_cast<std::int64_t>(quantityToFill * fillPrice) : 
            static_cast<std::int64_t>(quantityToFill * fillPrice)};

        if (!userId.is_nil()) {
            database::createTransaction(userId, shareId, transactionQuantity, fillPrice);
            database::updateUserHoldings(userId, shareId, transactionQuantity);
            database::updateUserBalance(userId, transactionValue);
        }
        else {
            database::updateIssuedShares(eventId, -transactionQuantity);
        }
        
        return quantityToFill;
    }

    Order::~Order() {
        
    }
}