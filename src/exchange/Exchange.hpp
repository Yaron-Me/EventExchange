#pragma once

#include <vector>

#include "Stock.hpp"

namespace exchange {
    class Exchange {
        public:
            bool createStock(const boost::uuids::uuid& stockId,
                             const boost::uuids::uuid& yesId,
                             const boost::uuids::uuid& noId);

            const std::vector<Stock>& getStocks() const {
                return stocks;
            }

        private:
            std::vector<Stock> stocks;
    };
}