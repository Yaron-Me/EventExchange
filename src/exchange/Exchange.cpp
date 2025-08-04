#include "Exchange.hpp"
#include "../database/exchange.hpp"

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

    crow::response Exchange::createOrder(const boost::uuids::uuid& userId,
                               const boost::uuids::uuid& stockId,
                               const boost::uuids::uuid& shareId,
                               OrderType type, std::uint32_t quantity, std::uint16_t price) {

        std::cout << "Creating order for user: " << utility::uuidToString(userId)
                  << ", stock: " << utility::uuidToString(stockId)
                  << ", share: " << utility::uuidToString(shareId)
                  << ", type: " << static_cast<int>(type)
                  << ", quantity: " << quantity
                  << ", price: " << price
                  << "\n";

        return crow::response{400, "Order creation not implemented"};
    }

    Exchange::Exchange() {
        for (const auto& data : database::getStocks()) {
            Stock stock{data.id, data.yesShare.id, data.noShare.id};
            stocks.push_back(stock);
        }
    }
}