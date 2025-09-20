#include <print>
#include <algorithm>

#include "event.hpp"

namespace engine {
    Event::Event(const boost::uuids::uuid& _id,
                 const boost::uuids::uuid& _yesId,
                 const boost::uuids::uuid& _noId) :
        id{_id}, yes{_yesId}, no{_noId} {}

    void Event::printOrderbook() const {
        std::print("\033[34mYES SHARE ORDERBOOK\n");
        std::print("\033[32mBUY ORDERS:\n");
        for (auto [price, quantity] : yes.getBuyPricesAndQuantities()) {
            std::print("Price: {} ({}), Quantity: {}\n", price, MAX_DENOMINATIONS - price, quantity); // green for buys
        }
        std::print("\033[31mSELL ORDERS:\n");
        for (auto [price, quantity] : yes.getSellPricesAndQuantities()) {
            std::print("Price: {} ({}), Quantity: {}\n", price, MAX_DENOMINATIONS - price, quantity);
        }

        std::print("\n\033[34mNO SHARE ORDERBOOK\n");
        std::print("\033[32mBUY ORDERS:\n");
        for (auto [price, quantity] : no.getBuyPricesAndQuantities()) {
            std::print("Price: {} ({}), Quantity: {}\n", price, MAX_DENOMINATIONS - price, quantity); // green for buys
        }
        std::print("\033[31mSELL ORDERS:\n");
        for (auto [price, quantity] : no.getSellPricesAndQuantities()) {
            std::print("Price: {} ({}), Quantity: {}\n", price, MAX_DENOMINATIONS - price, quantity);
        }
        std::print("\033[0m");
    }

    void Event::cancelOrder(std::shared_ptr<Order>& order) {
        ((order->shareId == yes.id) ? yes : no).cancelOrder(order);
    }

    std::pair<std::uint64_t, std::uint64_t> Event::getBuyAndSellQuantities(const boost::uuids::uuid& shareId) const {
        const Share& thisShare{(shareId == yes.id) ? yes : no};
        return {thisShare.getBuyOrderQuantity(), thisShare.getSellOrderQuantity()};
    }

    void Event::addOrder(std::shared_ptr<Order>& order) {
        // Before we add the order we have to first match it with all other orders
        Share& thisShare{(order->shareId == yes.id) ? yes : no};
        Share& otherShare{(order->shareId == yes.id) ? no : yes};

        if (order->type == OrderType::BUY) {
            auto sellPricesAndQuantities{thisShare.getSellPricesAndQuantities(order->leftoverQuantitiy(), order->price)};
            auto matchPricesAndQuantities{otherShare.getBuyPricesAndQuantities(order->leftoverQuantitiy(), MAX_DENOMINATIONS - order->price, true)};

            uint64_t toFind{order->leftoverQuantitiy()};
            uint64_t found{0};
            uint64_t synthFound{0};

            size_t i{0};
            size_t j{0};
            while (found < toFind) {
                if (sellPricesAndQuantities.size() > i && matchPricesAndQuantities.size() > j) {
                    auto [p1, q1]{sellPricesAndQuantities[i]};
                    auto [p2, q2]{matchPricesAndQuantities[j]};

                    if (p1 < p2) {
                        // buy fills sell
                        auto t{std::min(q1, toFind - found)};
                        found += t;
                        i++;
                    }
                    else if (p2 < p1) {
                        // buy match buy
                        auto t{std::min(q2, toFind - found)};
                        found += t;
                        synthFound += t;
                        order->fill(t, p2);
                        j++;
                    }
                    else {
                        if (q1 >= q2) {
                            // buy fills sell
                            auto t{std::min(q1, toFind - found)};
                            found += t;
                            i++;
                        }
                        else {
                            // buy match buy
                            auto t{std::min(q2, toFind - found)};
                            found += t;
                            synthFound += t;
                            order->fill(t, p2);
                            j++;
                        }
                    }
                }
                else if (sellPricesAndQuantities.size() > i) {
                    // buy fills sell
                    auto [p1, q1]{sellPricesAndQuantities[i]};
                    auto t{std::min(q1, toFind - found)};
                    found += t;
                    i++;
                }
                else if (matchPricesAndQuantities.size() > j) {
                    // buy match buy
                    auto [p2, q2]{matchPricesAndQuantities[j]};
                    auto t{std::min(q2, toFind - found)};
                    found += t;
                    synthFound += t;
                    order->fill(t, p2);
                    j++;
                }
                else {
                    break;
                }
            }
            otherShare.fillOrder(std::make_shared<Order>(boost::uuids::uuid{}, OrderType::SELL, OrderMode::MARKET,
                                             order->eventId, otherShare.id,
                                             synthFound, MAX_DENOMINATIONS - order->price));
            thisShare.fillOrder(order);
        }
        // else if (order->type == OrderType::SELL) {
        //     auto buyPricesAndQuantities = thisShare.getBuyPricesAndQuantities(order->leftoverQuantitiy(), order->price);
        //     auto sellPricesAndQuantities = otherShare.getSellPricesAndQuantities(order->leftoverQuantitiy(), MAX_DENOMINATIONS - order->price, true);
        // }

        if (order->mode == OrderMode::LIMIT && order->leftoverQuantitiy() > 0) {
            thisShare.addOrder(order);
        }
    }
}