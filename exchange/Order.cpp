#include "exchange/Order.hpp"
#include "utility/UUID.hpp"

namespace exchange {
    Order::Order(OrderQueue& _parentQueue, User& _parentUser,
                 const boost::uuids::uuid& _ownerId, const OrderType _type,
                 const std::string& _stockId, const std::string& _shareId,
                 const std::uint32_t _quantity, const std::uint16_t _price) :
        id(utility::generateUUID()), ownerId(_ownerId), type(_type),
        stockId(_stockId), shareId(_shareId), quantity(_quantity), price(_price),
        filledQuantity(0), fillCosts(0),
        parentQueue(_parentQueue), parentUser(_parentUser)
    {
        assert(_price <= MAX_DENOMINATIONS);
    }

    Order::~Order() {
        // Destructor
    }
}