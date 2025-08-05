#pragma once

#include <vector>
#include <crow.h>

#include "Event.hpp"
#include "User.hpp"

namespace exchange {
    class Exchange {
        public:
            bool createEvent(const boost::uuids::uuid& eventId,
                             const boost::uuids::uuid& yesId,
                             const boost::uuids::uuid& noId);

            crow::response createOrder(const boost::uuids::uuid& userId,
                             const boost::uuids::uuid& eventId,
                             const boost::uuids::uuid& shareId,
                             OrderType type, std::uint32_t quantity, std::uint16_t price);

            User& getUser(const boost::uuids::uuid& userId) {
                return users[userId];
            }

            Exchange();

        private:
            std::map<boost::uuids::uuid, Event> events;
            std::map<boost::uuids::uuid, User> users;
    };
}