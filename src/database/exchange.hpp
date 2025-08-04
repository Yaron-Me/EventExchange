#pragma once

#include <string>
#include <vector>

#include "../exchange/Exchange.hpp"
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
        ShareData yesShare;
        ShareData noShare;
    };

    bool createEvent(exchange::Exchange& exchange, const std::string& eventName,
                     const std::string& eventDescription, const std::string& yesShareName,
                     const std::string& noShareName);
    const std::vector<EventData> getEvents();
}