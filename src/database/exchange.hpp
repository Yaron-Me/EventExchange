#pragma once

#include <string>
#include <vector>

#include "../exchange/Exchange.hpp"
#include "../utility/uuid.hpp"

namespace database {
    struct ShareData {
        boost::uuids::uuid id;
        std::string name;
    };

    struct StockData {
        boost::uuids::uuid id;
        std::string name;
        std::string description;
        std::string createdAt;
        ShareData yesShare;
        ShareData noShare;
    };

    bool createStock(exchange::Exchange& exchange, const std::string& stockName,
                     const std::string& stockDescription, const std::string& yesShareName,
                     const std::string& noShareName);
    const std::vector<StockData> getStocks();
}