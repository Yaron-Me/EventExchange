#pragma once

#include <vector>
#include <crow.h>

#include "event.hpp"
#include "user.hpp"

namespace engine {
    class Exchange {
        public:
            void createEvent(const boost::uuids::uuid& eventId,
                             const boost::uuids::uuid& yesId,
                             const boost::uuids::uuid& noId);

            std::tuple<bool, std::string> createOrder(const boost::uuids::uuid& userId,
                             const boost::uuids::uuid& eventId,
                             const boost::uuids::uuid& shareId,
                             const OrderType type, const OrderMode mode,
                             const std::uint32_t quantity, const std::uint16_t price);

            User& getUser(const boost::uuids::uuid& userId);

            Exchange();

        private:
            std::map<boost::uuids::uuid, Event> events;
            std::map<boost::uuids::uuid, User> users;

            void cleanupUser(const boost::uuids::uuid& userId);

            bool addOrder(std::shared_ptr<Order>& order);
    };
}