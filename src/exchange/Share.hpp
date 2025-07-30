#pragma once

#include <boost/uuid.hpp>
#include <map>

#include "OrderQueue.hpp"
#include "Order.hpp"

namespace exchange {
    class Share {
        public:
            const boost::uuids::uuid id;

            Share(const boost::uuids::uuid& _id);

        private:
            std::map<int, OrderQueue> buyOrders;
            std::map<int, OrderQueue> sellOrders;
    };
}