#pragma once

#include <boost/uuid.hpp>
#include <string>
#include "../exchange/Order.hpp"

namespace utility {
    const boost::uuids::uuid generateUUID();
    const std::string uuidToString(const boost::uuids::uuid& uuid);
    const boost::uuids::uuid stringToUUID(const std::string& uuidStr);
}
