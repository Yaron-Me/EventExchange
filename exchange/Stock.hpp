#pragma once

#include <boost/uuid.hpp>
#include <vector>

#include "exchange/Share.hpp"
#include "exchange/Order.hpp"

namespace exchange {
    class Stock {
        public:
            const boost::uuids::uuid id;

            Stock(const boost::uuids::uuid _id);

            bool createShare(const boost::uuids::uuid& shareId);

        private:
            std::vector<Share> shares;
    };
}