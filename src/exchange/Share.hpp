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

            void addOrder(std::shared_ptr<Order> order);

        private:
            std::map<int, OrderQueue> buyOrders;
            std::map<int, OrderQueue> sellOrders;
    };
}