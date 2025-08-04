#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <map>

#include "../utility/uuid.hpp"

#include "exchange.hpp"

namespace database {
    bool createStock(exchange::Exchange& exchange, const std::string& stockName,
                     const std::string& stockDescription, const std::string& yesShareName,
                     const std::string& noShareName) {
        const auto stockId{utility::generateUUID()};
        const auto yesShareId{utility::generateUUID()};
        const auto noShareId{utility::generateUUID()};

        const std::string stockIdStr{utility::uuidToString(stockId)};
        const std::string yesShareIdStr{utility::uuidToString(yesShareId)};
        const std::string noShareIdStr{utility::uuidToString(noShareId)};

        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READWRITE};
            
            SQLite::Transaction transaction{db};
            
            SQLite::Statement stockQuery{db, "INSERT INTO stocks (id, name, description) VALUES (?, ?, ?)"};
            stockQuery.bind(1, stockIdStr);
            stockQuery.bind(2, stockName);
            stockQuery.bind(3, stockDescription);
            stockQuery.exec();
            
            SQLite::Statement yesShareQuery{db, "INSERT INTO shares (id, name, stock_id, type) VALUES (?, ?, ?, 'YES')"};
            yesShareQuery.bind(1, yesShareIdStr);
            yesShareQuery.bind(2, yesShareName);
            yesShareQuery.bind(3, stockIdStr);
            yesShareQuery.exec();
            
            SQLite::Statement noShareQuery{db, "INSERT INTO shares (id, name, stock_id, type) VALUES (?, ?, ?, 'NO')"};
            noShareQuery.bind(1, noShareIdStr);
            noShareQuery.bind(2, noShareName);
            noShareQuery.bind(3, stockIdStr);
            noShareQuery.exec();

            transaction.commit();

            exchange.createStock(stockId, yesShareId, noShareId);
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating stock: " << e.what() << '\n';
            return false;
        }
    }

    const std::vector<StockData> getStocks() {
        std::map<std::string, StockData> stocksMap;

        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READONLY};

            SQLite::Statement query{db, R"(
                SELECT
                    s.id, s.name, s.description, s.created_at,
                    sh.id, sh.name, sh.type
                FROM
                    stocks s
                JOIN
                    shares sh ON s.id = sh.stock_id
                WHERE
                    s.settled = 0
                ORDER BY
                    s.created_at DESC;
            )"};

            while (query.executeStep()) {
                const auto stockIdStr{query.getColumn(0).getString()};
                const auto stockName{query.getColumn(1).getString()};
                const auto description{query.getColumn(2).getString()};
                const auto createdAt{query.getColumn(3).getString()};
                const auto shareIdStr{query.getColumn(4).getString()};
                const auto shareName{query.getColumn(5).getString()};
                const auto shareType{query.getColumn(6).getString()};

                if (stocksMap.find(stockIdStr) == stocksMap.end()) {
                    stocksMap[stockIdStr].id = utility::stringToUUID(stockIdStr);
                    stocksMap[stockIdStr].name = stockName;
                    stocksMap[stockIdStr].description = description;
                    stocksMap[stockIdStr].createdAt = createdAt;
                }

                if (shareType == "YES") {
                    stocksMap[stockIdStr].yesShare = {utility::stringToUUID(shareIdStr), shareName};
                } else if (shareType == "NO") {
                    stocksMap[stockIdStr].noShare = {utility::stringToUUID(shareIdStr), shareName};
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error fetching stocks: " << e.what() << '\n';
        }

        std::vector<StockData> results;
        results.reserve(stocksMap.size());
        for (const auto& [id, stock] : stocksMap) {
            results.push_back(stock);
        }

        return results;
    }
}