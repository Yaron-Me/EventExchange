#pragma once

#include <boost/uuid.hpp>
#include <vector>

#include "share.hpp"
#include "order.hpp"

namespace engine {
    class Event {
        public:
            const boost::uuids::uuid id;

            Event(const boost::uuids::uuid& _id,
                  const boost::uuids::uuid& _yesId,
                  const boost::uuids::uuid& _noId);

            void addOrder(std::shared_ptr<Order>& order);
            void cancelOrder(std::shared_ptr<Order>& order);

            void printOrderbook() const;

            std::pair<std::uint64_t, std::uint64_t> getBuyAndSellQuantities(const boost::uuids::uuid& shareId) const;

        private:
            Share yes;
            Share no;
    };
}