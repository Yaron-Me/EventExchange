#pragma once

#include <string>
#include <vector>

#include "../engine/exchange.hpp"
#include "../utility/uuid.hpp"

namespace database {
    struct ShareData {
        boost::uuids::uuid id;
        std::string name;
    };

    struct EventData {
        boost::uuids::uuid id;
        std::string name;
        std::string description;
        std::string createdAt;
        uint64_t issued;
        ShareData yesShare;
        ShareData noShare;
    };

    std::tuple<bool, EventData> createEvent(const std::string& eventName,
                     const std::string& eventDescription, const std::string& yesShareName,
                     const std::string& noShareName);
    const std::vector<EventData> getEvents();
    const EventData getEvent(const boost::uuids::uuid& eventId);
    void updateIssuedShares(const boost::uuids::uuid& eventId, std::int64_t quantity);
}