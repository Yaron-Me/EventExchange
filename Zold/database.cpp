#include <string>
#include <iostream>

#include "exchange.hpp"
#include "database.hpp"

namespace database {

void grantShares(const std::string& userId, const std::string& stockName,
    const std::string& shareName, int amount) {
    // Placeholder for granting shares to the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)stockName;
    (void)shareName;
    (void)amount;
}

void grantFunds(const std::string& userId, int amount) {
    // Placeholder for granting funds to the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)amount;
}

void removeShares(const std::string& userId, const std::string& stockName,
    const std::string& shareName, int amount) {
    // Placeholder for removing shares from the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)stockName;
    (void)shareName;
    (void)amount;
}

void removeFunds(const std::string& userId, int amount) {
    // Placeholder for removing funds from the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)amount;
}

void uploadFinishedOrder(const exchange::Order* order) {
    // Placeholder for uploading the order to the database
    // In a real implementation, this would update the order status in db
    std::string buysell = (order->type == exchange::OrderType::BUY) ? "bought" : "sold";

    if (order->isFilled()) {
        std::cout << "Order of user " << order->userId << " has been filled with " <<
        order->orderedAmount << " shares of " << order->shareName << " bought at an average price of " <<
        order->getAveragePrice() << std::endl;
    }
    else {
        std::cout << "Order of user " << order->userId << " has been cancelled with " <<
        order->orderedAmount - order->getLeftToOrder() << "/" << order->orderedAmount << " shares of " << order->shareName << " bought at an average price of " <<
        order->getAveragePrice() << std::endl;
    }

    (void)order;
}

}