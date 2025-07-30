#include "exchange/Exchange.hpp"

namespace exchange {
    bool Exchange::createStock(const boost::uuids::uuid& stockId) {
        // Check if the stock already exists
        for (const auto& stock : stocks) {
            if (stock.id == stockId) {
                return false; // Stock already exists
            }
        }
        // Create a new stock and add it to the exchange
        stocks.emplace_back(stockId);
        return true; // Stock created successfully
    }
}