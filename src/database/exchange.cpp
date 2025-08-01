#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>

#include "../utility/uuid.hpp"
#include "exchange.hpp"

namespace database {
    bool createStock(const std::string& stockName, const std::string& stockDescription,
                     const std::string& yesShareName, const std::string& noShareName) {
        const auto stockId{utility::generateUUID()};
        const auto yesShareId{utility::generateUUID()};
        const auto noShareId{utility::generateUUID()};

        const std::string stockIdStr{utility::uuidToString(stockId)};
        const std::string yesShareIdStr{utility::uuidToString(yesShareId)};
        const std::string noShareIdStr{utility::uuidToString(noShareId)};

        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READWRITE};
            
            SQLite::Transaction transaction{db};
            
            SQLite::Statement stockQuery{db, "INSERT INTO stocks (id, name, description, yes_share_id, no_share_id) VALUES (?, ?, ?, ?, ?)"};
            stockQuery.bind(1, stockIdStr);
            stockQuery.bind(2, stockName);
            stockQuery.bind(3, stockDescription);
            stockQuery.bind(4, yesShareIdStr);
            stockQuery.bind(5, noShareIdStr);
            stockQuery.exec();
            
            SQLite::Statement yesShareQuery{db, "INSERT INTO shares (id, name, stock_id) VALUES (?, ?, ?)"};
            yesShareQuery.bind(1, yesShareIdStr);
            yesShareQuery.bind(2, yesShareName);
            yesShareQuery.bind(3, stockIdStr);
            yesShareQuery.exec();
            
            SQLite::Statement noShareQuery{db, "INSERT INTO shares (id, name, stock_id) VALUES (?, ?, ?)"};
            noShareQuery.bind(1, noShareIdStr);
            noShareQuery.bind(2, noShareName);
            noShareQuery.bind(3, stockIdStr);
            noShareQuery.exec();
            
            transaction.commit();
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating stock: " << e.what() << std::endl;
            return false;
        }
    }



}