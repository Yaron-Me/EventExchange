#pragma once

#include <vector>

#include "exchange/Stock.hpp"

namespace exchange {
    class Exchange {
        public:
            bool createStock(const boost::uuids::uuid& stockId);

        private:
            std::vector<Stock> stocks;
    };
}