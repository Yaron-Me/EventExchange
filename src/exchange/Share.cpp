#include <boost/uuid.hpp>
#include <print>
#include <iostream>

#include "share.hpp"

namespace exchange {
    Share::Share(const boost::uuids::uuid& _id) : id{_id} {}

    void Share::addOrder(std::shared_ptr<Order> order) {
        if (order->type == OrderType::BUY) {
            const auto price{order->price};
            if (!buyOrders[price - 1]) {
                buyOrders[price - 1] = std::make_unique<OrderQueue>();
            }
            buyOrders[price - 1]->addOrder(order);
            buyOrderQuantity += order->leftoverQuantitiy();
        }
        else if (order->type == OrderType::SELL) {
            const auto price{order->price};
            if (!sellOrders[price - 1]) {
                sellOrders[price - 1] = std::make_unique<OrderQueue>();
            }
            sellOrders[price - 1]->addOrder(order);
            sellOrderQuantity += order->leftoverQuantitiy();
        }
    }

    void Share::cancelOrder(std::shared_ptr<Order> order) {
        if (order->type == OrderType::BUY) {
            const auto price{order->price};
            if (buyOrders[price - 1]) {
                buyOrders[price - 1]->cancelOrder(order);
                if (buyOrders[price - 1]->getOrderCount() == 0) {
                    buyOrders[price - 1].reset();
                }
            }
            buyOrderQuantity -= order->leftoverQuantitiy();
        }
        else if (order->type == OrderType::SELL) {
            const auto price{order->price};
            if (sellOrders[price - 1]) {
                sellOrders[price - 1]->cancelOrder(order);
                if (sellOrders[price - 1]->getOrderCount() == 0) {
                    sellOrders[price - 1].reset();
                }
            }
            sellOrderQuantity -= order->leftoverQuantitiy();
        }
    }

    std::vector<std::pair<std::uint16_t, std::uint64_t>> Share::getBuyPricesAndQuantities() const {
        std::vector<std::pair<std::uint16_t, std::uint64_t>> pricesAndQuantities;
        for (std::uint16_t price = 1; price <= MAX_DENOMINATIONS; ++price) {
            if (buyOrders[price - 1]) {
                pricesAndQuantities.emplace_back(price, buyOrders[price - 1]->getTotalQuantity());
            }
        }
        return pricesAndQuantities;
    }

    std::vector<std::pair<std::uint16_t, std::uint64_t>> Share::getSellPricesAndQuantities() const {
        std::vector<std::pair<std::uint16_t, std::uint64_t>> pricesAndQuantities;
        for (std::uint16_t price = 1; price <= MAX_DENOMINATIONS; ++price) {
            if (sellOrders[price - 1]) {
                pricesAndQuantities.emplace_back(price, sellOrders[price - 1]->getTotalQuantity());
            }
        }
        return pricesAndQuantities;
    }
}