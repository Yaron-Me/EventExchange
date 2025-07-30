#include "Exchange.hpp"

namespace exchange {
    bool Exchange::createStock(const boost::uuids::uuid& stockId,
                               const boost::uuids::uuid& yesId,
                               const boost::uuids::uuid& noId) {
        // Check if the stock already exists
        for (const auto& stock : stocks) {
            if (stock.id == stockId) {
                return false; // Stock already exists
            }
        }
        // Create a new stock and add it to the exchange
        stocks.emplace_back(stockId, yesId, noId);
        return true; // Stock created successfully
    }
}