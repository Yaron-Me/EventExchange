#pragma once

#include <boost/uuid.hpp>
#include <vector>

#include "Share.hpp"
#include "Order.hpp"

namespace exchange {
    class Event {
        public:
            const boost::uuids::uuid id;

            Event(const boost::uuids::uuid& _id,
                  const boost::uuids::uuid& _yesId,
                  const boost::uuids::uuid& _noId);

            void addOrder(std::shared_ptr<Order> order);

        private:
            Share yes;
            Share no;

            void Event::addBuyOrder(std::shared_ptr<Order> order);
            void Event::addSellOrder(std::shared_ptr<Order> order);
    };
}