#pragma once

#include <string>
#include <boost/uuid/uuid.hpp>
#include <map>

namespace database {
    struct TransactionData {
        boost::uuids::uuid id;
        boost::uuids::uuid userId;
        boost::uuids::uuid shareId;
        std::int64_t amount;
        std::uint16_t price;
        std::string createdAt;
    };

    std::tuple<bool, boost::uuids::uuid> registerUser(const std::string& username, const std::string& password);

    std::int64_t getUserBalance(const boost::uuids::uuid& userId);
    void updateUserBalance(const boost::uuids::uuid& userId, std::int64_t amount);

    std::uint32_t getUserShareHoldings(const boost::uuids::uuid& userId, const boost::uuids::uuid& shareId);
    std::map<boost::uuids::uuid, std::uint32_t> getUserHoldings(const boost::uuids::uuid& userId);
    void updateUserHoldings(const boost::uuids::uuid& userId, const boost::uuids::uuid& shareId, std::int64_t amount);

    void createTransaction(const boost::uuids::uuid& userId, const boost::uuids::uuid& shareId, std::int64_t amount, std::uint16_t price);
    std::vector<TransactionData> getUserTransactions(const boost::uuids::uuid& userId, std::size_t offset = 0, std::size_t limit = 100);
}