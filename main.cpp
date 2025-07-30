#include <boost/uuid.hpp>
#include <iostream>

#include "utility/UUID.hpp"
#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"

int main() {
    exchange::Exchange exchange;

    // Create a new stock
    boost::uuids::uuid stockId = utility::generateUUID();
    if (exchange.createStock(stockId)) {
        std::cout << "Stock created successfully: " << stockId << std::endl;
    } else {
        std::cout << "Failed to create stock (already exists): " << stockId << std::endl;
    }

    return 0;
}