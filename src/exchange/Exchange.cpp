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
                               OrderType type, std::uint32_t quantity, std::uint16_t price) {

        std::cout << "Creating order for user: " << utility::uuidToString(userId)
                  << ", event: " << utility::uuidToString(eventId)
                  << ", share: " << utility::uuidToString(shareId)
                  << ", type: " << type
                  << ", quantity: " << quantity
                  << ", price: " << price
                  << "\n";

        auto& user = getUser(userId);
        auto userBalance = database::getUserBalance(userId);
        auto positionsValue = user.getPositionsValue();
        auto positionValue = quantity * price;
        auto userHoldings = database::getUserHoldings(userId);

        if (type == OrderType::BUY) {
            // Check if user has enough balance
            if (userBalance - positionsValue < positionValue) {
                return crow::response{400, "Insufficient balance for order"};
            }
        }
        else if (type == OrderType::SELL) {
            // Check if user has enough shares to sell
            auto holdingsIt = userHoldings.find(shareId);
            if (holdingsIt == userHoldings.end() || holdingsIt->second < quantity) {
                return crow::response{400, "Insufficient shares to sell"};
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