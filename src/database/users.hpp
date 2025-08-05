#pragma once

#include <string>
#include <boost/uuid/uuid.hpp>

namespace database {
    bool registerUser(const std::string& username, const std::string& password);
    std::int64_t getUserBalance(const boost::uuids::uuid& userId);
    std::map<boost::uuids::uuid, std::uint32_t> getUserHoldings(const boost::uuids::uuid& userId);
    void increaseUserBalance(const boost::uuids::uuid& userId, std::int64_t amount);
}