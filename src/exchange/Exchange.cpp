#include "Exchange.hpp"
#include "../database/exchange.hpp"

namespace exchange {
    bool Exchange::createEvent(const boost::uuids::uuid& eventId,
                               const boost::uuids::uuid& yesId,
                               const boost::uuids::uuid& noId) {
        // Check if the event already exists
        for (const auto& event : events) {
            if (event.id == eventId) {
                return false; // Event already exists
            }
        }
        // Create a new event and add it to the exchange
        events.emplace_back(eventId, yesId, noId);
        return true; // Event created successfully
    }

    crow::response Exchange::createOrder(const boost::uuids::uuid& userId,
                               const boost::uuids::uuid& eventId,
                               const boost::uuids::uuid& shareId,
                               OrderType type, std::uint32_t quantity, std::uint16_t price) {

        std::cout << "Creating order for user: " << utility::uuidToString(userId)
                  << ", event: " << utility::uuidToString(eventId)
                  << ", share: " << utility::uuidToString(shareId)
                  << ", type: " << static_cast<int>(type)
                  << ", quantity: " << quantity
                  << ", price: " << price
                  << "\n";

        return crow::response{400, "Order creation not implemented"};
    }

    Exchange::Exchange() {
        for (const auto& data : database::getEvents()) {
            Event event{data.id, data.yesShare.id, data.noShare.id};
            events.push_back(event);
        }
    }
}