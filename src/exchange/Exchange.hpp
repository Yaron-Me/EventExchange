#pragma once

#include <vector>
#include <crow.h>

#include "Stock.hpp"

namespace exchange {
    class Exchange {
        public:
            bool createStock(const boost::uuids::uuid& stockId,
                             const boost::uuids::uuid& yesId,
                             const boost::uuids::uuid& noId);

            crow::response createOrder(const boost::uuids::uuid& userId,
                             const boost::uuids::uuid& stockId,
                             const boost::uuids::uuid& shareId,
                             OrderType type, std::uint32_t quantity, std::uint16_t price);

            const std::vector<Stock>& getStocks() const {
                return stocks;
            }

        private:
            std::vector<Stock> stocks;
    };
}