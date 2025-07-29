#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <boost/uuid.hpp>

namespace exchange {
    static const std::uint16_t MAX_DENOMINATIONS = 100;

    enum class OrderType {
        BUY,
        SELL
    };

    class Order {
        public:
            const boost::uuids::uuid id;
            const boost::uuids::uuid ownerId;
            const OrderType type;
            const std::string stockId;
            const std::string shareId;
            const std::uint32_t quantity;
            const std::uint16_t price;

            Order(const boost::uuids::uuid& _ownerId, OrderType _type,
                  const std::string& _stockId, const std::string& _shareId,
                  std::uint32_t _quantity, std::uint16_t _price);

            ~Order();

        private:
            
    };
}