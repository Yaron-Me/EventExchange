#include <boost/uuid.hpp>
#include <print>
#include <iostream>

#include "share.hpp"

namespace exchange {
    Share::Share(const boost::uuids::uuid& _id) : id{_id} {}

    void Share::addOrder(std::shared_ptr<Order> order) {
        const auto price{order->price};
        if (order->type == OrderType::BUY) {
            if (!buyOrders[price - 1]) {
                buyOrders[price - 1] = std::make_unique<OrderQueue>();
            }
            buyOrders[price - 1]->addOrder(order);
            buyOrderQuantity += order->leftoverQuantitiy();
        }
        else if (order->type == OrderType::SELL) {
            if (!sellOrders[price - 1]) {
                sellOrders[price - 1] = std::make_unique<OrderQueue>();
            }
            sellOrders[price - 1]->addOrder(order);
            sellOrderQuantity += order->leftoverQuantitiy();
        }
    }

    // Returns filled amount
    std::uint64_t Share::fillOrder(std::shared_ptr<Order> order) {
        std::uint64_t totalFilled{0};

        if (order->type == OrderType::BUY) {
            for (auto [price, quantity] : getSellPricesAndQuantities()) {
                if (order->leftoverQuantitiy() == 0 ||
                    price > order->price) break;

                const auto filled{sellOrders[price - 1]->fillOrder(order)};
                if (sellOrders[price - 1]->getOrderCount() == 0) {
                    sellOrders[price - 1].reset();
                }
                sellOrderQuantity -= filled;
                totalFilled += filled;
            }
        }
        else if (order->type == OrderType::SELL) {
            for (auto [price, quantity] : getBuyPricesAndQuantities()) {
                if (order->leftoverQuantitiy() == 0 ||
                    price < order->price) break;

                const auto filled{buyOrders[price - 1]->fillOrder(order)};
                if (buyOrders[price - 1]->getOrderCount() == 0) {
                    buyOrders[price - 1].reset();
                }
                buyOrderQuantity -= filled;
                totalFilled += filled;
            }
        }

        return totalFilled;
    }

    void Share::cancelOrder(std::shared_ptr<Order> order) {
        const auto price{order->price};

        if (order->type == OrderType::BUY) {
            if (buyOrders[price - 1]) {
                buyOrders[price - 1]->cancelOrder(order);
                if (buyOrders[price - 1]->getOrderCount() == 0) {
                    buyOrders[price - 1].reset();
                }
            }
            buyOrderQuantity -= order->leftoverQuantitiy();
        }
        else if (order->type == OrderType::SELL) {
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
        for (std::uint16_t price{MAX_DENOMINATIONS}; price >= 1; --price) {
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

    std::uint64_t Share::getBuyOrderQuantity() const {
        return buyOrderQuantity;
    }

    std::uint64_t Share::getSellOrderQuantity() const {
        return sellOrderQuantity;
    }
}