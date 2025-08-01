#pragma once

#include <boost/uuid.hpp>

namespace utility {
    const boost::uuids::uuid generateUUID();
    const std::string uuidToString(const boost::uuids::uuid& uuid);
}
