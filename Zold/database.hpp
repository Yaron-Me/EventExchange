#pragma once

#include <string>
#include "exchange.hpp"

namespace database {

// Grant shares to a user's account
void grantShares(const std::string& userId, const std::string& stockName,
                 const std::string& shareName, int amount);

// Grant funds to a user's account
void grantFunds(const std::string& userId, int amount);

// Remove shares from a user's account
void removeShares(const std::string& userId, const std::string& stockName,
                 const std::string& shareName, int amount);

// Remove funds from a user's account
void removeFunds(const std::string& userId, int amount);

// Upload a completed order to the database
void uploadFinishedOrder(const exchange::Order* order);

} // namespace database