#include <iostream>

#include "exchange.hpp"

int main() {
    // Create an order
    exchange::Exchange exchange;

    int STOCKS = 10;
    int SHARES = 3;
    int ORDERS = 10;
    int USERS = 10000;
    int AMOUNT = 10;

    int totalOrders = STOCKS * SHARES * ORDERS * USERS;
    std::cout << "Total orders: " << totalOrders << std::endl;

    for (int i = 0; i < STOCKS; ++i) {
        std::string stockName = "Stock" + std::to_string(i);
        std::vector<std::string> shareNames;
        for (int j = 0; j < SHARES; ++j) {
            shareNames.push_back("Share" + std::to_string(j));
        }
        exchange.createStock(stockName, shareNames);
    }
    for (int l = 0; l < ORDERS; ++l) {
        for (int i = 0; i < USERS; ++i) {
            std::string userId = "User" + std::to_string(i);
            for (int j = 0; j < STOCKS; ++j) {
                std::string stockName = "Stock" + std::to_string(j);
                for (int k = 0; k < SHARES; ++k) {
                    std::string shareName = "Share" + std::to_string(k);
                    int amount = AMOUNT;
                    int price = l + 1;
                    exchange.createOrder(userId, exchange::OrderType::BUY, stockName, shareName, amount, price);
                }
            }
        }
    }

    std::cout << "Total orders after creation: " << exchange.totalOrders() << std::endl;

    // Fill orders
    for (int l = 0; l < ORDERS; ++l) {
        for (int i = 0; i < USERS; ++i) {
            std::string userId = "User" + std::to_string(i);
            for (int j = 0; j < STOCKS; ++j) {
                std::string stockName = "Stock" + std::to_string(j);
                for (int k = 0; k < SHARES; ++k) {
                    std::string shareName = "Share" + std::to_string(k);
                    int amount = AMOUNT;
                    int price = ORDERS - l;
                    exchange.createOrder(userId, exchange::OrderType::SELL, stockName, shareName, amount, price);
                }
            }
        }
    }

    std::cout << "Total orders after filling: " << exchange.totalOrders() << std::endl;

    return 0;
}