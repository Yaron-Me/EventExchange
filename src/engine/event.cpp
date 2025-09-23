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
        fillOrder(order);

        if (order->mode == OrderMode::LIMIT && order->leftoverQuantitiy() > 0) {
            ((order->shareId == yes.id) ? yes : no).addOrder(order);
        }
    }

    void Event::fillOrder(std::shared_ptr<Order>& order) {
        Share& thisShare{(order->shareId == yes.id) ? yes : no};
        Share& otherShare{(order->shareId == yes.id) ? no : yes};

        if (order->type == OrderType::BUY) {
            auto sellPricesAndQuantities{thisShare.getSellPricesAndQuantities(order->leftoverQuantitiy(), order->price)};
            auto matchPricesAndQuantities{otherShare.getBuyPricesAndQuantities(order->leftoverQuantitiy(), MAX_DENOMINATIONS - order->price, true)};

            uint64_t toFind{order->leftoverQuantitiy()};
            uint64_t found{0};
            uint64_t synthFound{0};

            auto sellIt = sellPricesAndQuantities.begin();
            auto matchIt = matchPricesAndQuantities.begin();
            const auto sellEnd = sellPricesAndQuantities.end();
            const auto matchEnd = matchPricesAndQuantities.end();

            while (found < toFind && (sellIt != sellEnd || matchIt != matchEnd)) {
                bool chooseSell;
                std::vector<std::pair<std::uint16_t, std::uint64_t>>::iterator chosenIt;

                if (sellIt == sellEnd) {
                    chooseSell = false;
                    chosenIt = matchIt;
                }
                else if (matchIt == matchEnd) {
                    chooseSell = true;
                    chosenIt = sellIt;
                }
                else {
                    // Both available - compare and choose best
                    const auto sellPrice = sellIt->first;
                    const auto matchPrice = matchIt->first;

                    if (sellPrice < matchPrice) {
                        chooseSell = true;
                        chosenIt = sellIt;
                    }
                    else if (matchPrice < sellPrice) {
                        chooseSell = false;
                        chosenIt = matchIt;
                    }
                    else {
                        // Equal prices - prioritize by quantity
                        chooseSell = (sellIt->second >= matchIt->second);
                        chosenIt = chooseSell ? sellIt : matchIt;
                    }
                }

                const auto& [chosenPrice, chosenQuantity] = *chosenIt;
                auto fillAmount = std::min(chosenQuantity, toFind - found);
                found += fillAmount;

                if (!chooseSell) { // Synthetic match
                    synthFound += fillAmount;
                    order->fill(fillAmount, chosenPrice);
                }

                // Advance the chosen iterator
                if (chooseSell) ++sellIt; else ++matchIt;
            }

            if (synthFound > 0) {
                otherShare.fillOrder(std::make_shared<Order>(boost::uuids::uuid{}, OrderType::SELL, OrderMode::MARKET,
                                                 order->eventId, otherShare.id,
                                                 synthFound, MAX_DENOMINATIONS - order->price));
            }
            thisShare.fillOrder(order);
        }
        else if (order->type == OrderType::SELL) {
            auto buyPricesAndQuantities = thisShare.getBuyPricesAndQuantities(order->leftoverQuantitiy(), order->price);
            auto matchPricesAndQuantities = otherShare.getSellPricesAndQuantities(order->leftoverQuantitiy(), MAX_DENOMINATIONS - order->price, true);

            uint64_t toFind{order->leftoverQuantitiy()};
            uint64_t found{0};
            uint64_t synthFound{0};

            auto buyIt = buyPricesAndQuantities.begin();
            auto matchIt = matchPricesAndQuantities.begin();
            const auto buyEnd = buyPricesAndQuantities.end();
            const auto matchEnd = matchPricesAndQuantities.end();

            while (found < toFind && (buyIt != buyEnd || matchIt != matchEnd)) {
                bool chooseBuy;
                std::vector<std::pair<std::uint16_t, std::uint64_t>>::iterator chosenIt;

                if (buyIt == buyEnd) {
                    chooseBuy = false;
                    chosenIt = matchIt;
                }
                else if (matchIt == matchEnd) {
                    chooseBuy = true;
                    chosenIt = buyIt;
                }
                else {
                    // Both available - compare and choose best
                    const auto buyPrice = buyIt->first;
                    const auto matchPrice = matchIt->first;

                    if (buyPrice > matchPrice) {
                        chooseBuy = true;
                        chosenIt = buyIt;
                    }
                    else if (matchPrice > buyPrice) {
                        chooseBuy = false;
                        chosenIt = matchIt;
                    }
                    else {
                        // Equal prices - prioritize by quantity
                        chooseBuy = (buyIt->second >= matchIt->second);
                        chosenIt = chooseBuy ? buyIt : matchIt;
                    }
                }

                const auto& [chosenPrice, chosenQuantity] = *chosenIt;
                auto fillAmount = std::min(chosenQuantity, toFind - found);
                found += fillAmount;

                if (!chooseBuy) { // Synthetic match
                    synthFound += fillAmount;
                    order->fill(fillAmount, chosenPrice);
                }

                // Advance the chosen iterator
                if (chooseBuy) ++buyIt; else ++matchIt;
            }

            if (synthFound > 0) {
                otherShare.fillOrder(std::make_shared<Order>(boost::uuids::uuid{}, OrderType::BUY, OrderMode::MARKET,
                                                 order->eventId, otherShare.id,
                                                 synthFound, MAX_DENOMINATIONS - order->price));
            }
            thisShare.fillOrder(order);
        }
    }
}