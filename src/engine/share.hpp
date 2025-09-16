#pragma once

#include <boost/uuid.hpp>
#include <map>
#include <array>

#include "orderqueue.hpp"
#include "order.hpp"

namespace engine {
    class Share {
        public:
            const boost::uuids::uuid id;

            Share(const boost::uuids::uuid& _id);

            void addOrder(std::shared_ptr<Order> order);
            std::uint64_t fillOrder(std::shared_ptr<Order> order);
            void cancelOrder(std::shared_ptr<Order> order);

            std::uint64_t getBuyOrderQuantity() const;
            std::uint64_t getSellOrderQuantity() const;

            std::vector<std::pair<std::uint16_t, std::uint64_t>> getBuyPricesAndQuantities(std::uint64_t quantityLimit = 0) const;
            std::vector<std::pair<std::uint16_t, std::uint64_t>> getSellPricesAndQuantities(std::uint64_t quantityLimit = 0) const;

        private:
            std::array<std::unique_ptr<OrderQueue>, MAX_DENOMINATIONS> buyOrders;
            std::array<std::unique_ptr<OrderQueue>, MAX_DENOMINATIONS> sellOrders;

            std::uint64_t buyOrderQuantity{0};
            std::uint64_t sellOrderQuantity{0};
    };
}