#include <print>
#include <iostream>

#include "exchange.hpp"
#include "../database/exchange.hpp"
#include "../database/user.hpp"

namespace exchange {
    bool Exchange::createEvent(const boost::uuids::uuid& eventId,
                               const boost::uuids::uuid& yesId,
                               const boost::uuids::uuid& noId) {
        
        if (events.contains(eventId)) {
            return false;
        }

        events.emplace(eventId, Event{eventId, yesId, noId});
        return true;
    }

    User& Exchange::getUser(const boost::uuids::uuid& userId) {
        return users[userId];
    }

    std::tuple<bool, std::string> Exchange::createOrder(const boost::uuids::uuid& userId,
                                         const boost::uuids::uuid& eventId,
                                         const boost::uuids::uuid& shareId,
                                         const OrderType type, const OrderMode mode,
                                         const std::uint32_t quantity, const std::uint16_t price) {
        auto& user{getUser(userId)};

        if (type == OrderType::BUY) {
            const auto userBalance{database::getUserBalance(userId)};
            const auto tiedUpBalance{user.getTiedUpBalance()};
            const auto positionValue{quantity * price};
            if (userBalance - tiedUpBalance < positionValue) {
                cleanupUser(userId);
                return {false, "Insufficient balance for order"};
            }
        }
        else if (type == OrderType::SELL) {
            const auto userHoldings{database::getUserHoldings(userId)};
            const auto ownedShares{userHoldings.contains(shareId) ? userHoldings.at(shareId) : 0};
            const auto SellOrderShareCounts{user.getSellOrderShareCounts()};
            const auto sellShareCount{SellOrderShareCounts.contains(shareId) ? SellOrderShareCounts.at(shareId) : 0};
            if (ownedShares - sellShareCount < quantity) {
                cleanupUser(userId);
                return {false, "Insufficient holdings for order"};
            }
        }

        auto order{std::make_shared<Order>(userId, type, mode, eventId, shareId, quantity, price)};
        if (addOrder(order)) {
            user.addOrder(order);
        }
        else {
            cleanupUser(userId);
            return {false, "Order could not be added"};
        }
        
        return {true, "Order created successfully"};
    }

    Exchange::Exchange() {
        for (const auto& data : database::getEvents()) {
            events.emplace(data.id, Event{data.id, data.yesShare.id, data.noShare.id});
        }
    }

    void Exchange::cleanupUser(const boost::uuids::uuid& userId) {
        if (getUser(userId).getOrderCount() == 0) {
            users.erase(userId);
        }
    }

    bool Exchange::addOrder(std::shared_ptr<Order> order) {
        try {
            auto& event{events.at(order->eventId)};
            event.addOrder(order);
            return true;
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error adding order. This should never happen: {}\n", e.what());
            return false;
        }
    }
}