#pragma once

#include <vector>

#include "exchange/Stock.hpp"

namespace exchange {
    class Exchange {
        public:
            bool createStock(const boost::uuids::uuid& stockId,
                             const boost::uuids::uuid& yesId,
                             const boost::uuids::uuid& noId);

        private:
            std::vector<Stock> stocks;
    };
}