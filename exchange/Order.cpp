#include "exchange/Order.hpp"
#include "utility/UUID.hpp"

namespace exchange {
    Order::Order(const boost::uuids::uuid& _ownerId, OrderType _type,
                 const std::string& _stockId, const std::string& _shareId,
                 std::uint32_t _quantity, std::uint16_t _price) :
        id(utility::generateUUID()), ownerId(_ownerId), type(_type),
        stockId(_stockId), shareId(_shareId), quantity(_quantity), price(_price)
    {
        assert(_price <= MAX_DENOMINATIONS);
    }

    Order::~Order() {
        // Destructor
    }
}