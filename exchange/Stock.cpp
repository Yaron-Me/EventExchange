#include "exchange/Stock.hpp"

namespace exchange {
    Stock::Stock(const boost::uuids::uuid _id) : id(_id) {};

    bool Stock::createShare(const boost::uuids::uuid& shareId) {
        // Check if the share already exists
        for (const auto& share : shares) {
            if (share.id == shareId) {
                return false; // Share already exists
            }
        }
        // Create a new share and add it to the stock
        shares.emplace_back(shareId);
        return true; // Share created successfully
    }
}