#include "Exchange.hpp"
#include "../database/exchange.hpp"
#include "../database/users.hpp"

namespace exchange {
    bool Exchange::createEvent(const boost::uuids::uuid& eventId,
                               const boost::uuids::uuid& yesId,
                               const boost::uuids::uuid& noId) {
        
        if (events.find(eventId) != events.end()) {
            return false; // Event already exists
        }

        events.emplace(eventId, Event{eventId, yesId, noId});
        return true; // Event created successfully
    }

    crow::response Exchange::createOrder(const boost::uuids::uuid& userId,
                                         const boost::uuids::uuid& eventId,
                                         const boost::uuids::uuid& shareId,
                                         const OrderType type, const std::uint32_t quantity,
                                         const std::uint16_t price) {

        std::cout << "Creating order for user: " << utility::uuidToString(userId)
                  << ", event: " << utility::uuidToString(eventId)
                  << ", share: " << utility::uuidToString(shareId)
                  << ", type: " << type
                  << ", quantity: " << quantity
                  << ", price: " << price
                  << "\n";

        auto& user = getUser(userId);
        
        if (type == OrderType::BUY) {
            const auto userBalance = database::getUserBalance(userId);
            const auto tiedUpBalance = user.getTiedUpBalance();
            const auto positionValue = quantity * price;
            if (userBalance - tiedUpBalance < positionValue) {
                return crow::response{400, "Insufficient balance for order"};
            }
        }
        else if (type == OrderType::SELL) {
            const auto userHoldings = database::getUserHoldings(userId);
            const auto holdingsIt = userHoldings.find(shareId);
            const auto ownedShares = (holdingsIt != userHoldings.end()) ? holdingsIt->second : 0;
            const auto SellOrderShareCounts = user.getSellOrderShareCounts();
            const auto sellShareCountIt = SellOrderShareCounts.find(shareId);
            const auto sellShareCount = (sellShareCountIt != SellOrderShareCounts.end()) ? sellShareCountIt->second : 0;
            if (ownedShares - sellShareCount < quantity) {
                return crow::response{400, "Insufficient shares for order"};
            }
        }

        return crow::response{201, "Order created successfully"};
    }

    Exchange::Exchange() {
        for (const auto& data : database::getEvents()) {
            events.emplace(data.id, Event{data.id, data.yesShare.id, data.noShare.id});
        }
    }
}