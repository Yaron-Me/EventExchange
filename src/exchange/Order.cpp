#include "Order.hpp"
#include "../utility/uuid.hpp"

namespace exchange {
    Order::Order(OrderQueue& _parentQueue, User& _parentUser,
                 const boost::uuids::uuid& _ownerId, const OrderType _type,
                 const std::string& _stockId, const std::string& _shareId,
                 const std::uint32_t _quantity, const std::uint16_t _price) :
        id{utility::generateUUID()}, ownerId{_ownerId}, type{_type},
        stockId{_stockId}, shareId{_shareId}, quantity{_quantity}, price{_price},
        filledQuantity{0}, fillCosts{0},
        parentQueue{_parentQueue}, parentUser{_parentUser}
    {
        assert(_price <= MAX_DENOMINATIONS);
    }

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

    std::ostream& operator<<(std::ostream& os, const OrderType& type) {
        switch (type) {
            case OrderType::BUY:
                os << "BUY";
                break;
            case OrderType::SELL:
                os << "SELL";
                break;
        }
        return os; // Return the stream to allow chaining (e.g., std::cout << type << '\n';)
    }
}