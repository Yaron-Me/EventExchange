#include <SQLiteCpp/SQLiteCpp.h>
#include <print>
#include <iostream>

#include "user.hpp"
#include "utility.hpp"
#include "../utility/uuid.hpp"
#include "../utility/time.hpp"

namespace database {
    std::tuple<bool, boost::uuids::uuid> registerUser(const std::string& username, const std::string& password) {
        const auto userId{utility::generateUUID()};
        const auto userIdStr{utility::uuidToString(userId)};

        try {
            auto& db = getDatabase();
            
            SQLite::Statement query{db, "INSERT INTO users (id, name, password, balance) VALUES (?, ?, ?, ?)"};
            query.bind(1, userIdStr);
            query.bind(2, username);
            query.bind(3, password);
            query.bind(4, 0);
            
            if (query.exec() > 0) {
                return {true, userId};
            } else {
                std::print(std::cerr, "Error: Failed to register user, no rows affected\n");
                return {false, boost::uuids::uuid{}};
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error registering user: {}\n", e.what());
            return {false, boost::uuids::uuid{}};
        }
    }

    std::int64_t getUserBalance(const boost::uuids::uuid& userId) {
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, "SELECT balance FROM users WHERE id = ?"};
            query.bind(1, utility::uuidToString(userId));

            if (query.executeStep()) {
                return query.getColumn(0).getInt64();
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error fetching user balance: {}\n", e.what());
        }

        return 0;
    }

    std::uint32_t getUserShareHoldings(const boost::uuids::uuid& userId, const boost::uuids::uuid& shareId) {
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, "SELECT quantity FROM user_holdings WHERE user_id = ? AND share_id = ?"};
            query.bind(1, utility::uuidToString(userId));
            query.bind(2, utility::uuidToString(shareId));

            if (query.executeStep()) {
                return query.getColumn(0).getUInt();
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error fetching user share holdings: {}\n", e.what());
        }

        return 0;
    }

    std::map<boost::uuids::uuid, std::uint32_t> getUserHoldings(const boost::uuids::uuid& userId) {
        std::map<boost::uuids::uuid, std::uint32_t> holdings;
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, "SELECT share_id, quantity FROM user_holdings WHERE user_id = ?"};
            query.bind(1, utility::uuidToString(userId));

            while (query.executeStep()) {
                const auto shareId{utility::stringToUUID(query.getColumn(0).getText())};
                const auto quantity{query.getColumn(1).getUInt()};
                holdings[shareId] = quantity;
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error fetching user holdings: {}\n", e.what());
        }
        return holdings;
    }

    void updateUserBalance(const boost::uuids::uuid& userId, std::int64_t amount) {
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, "UPDATE users SET balance = balance + ? WHERE id = ?"};
            query.bind(1, amount);
            query.bind(2, utility::uuidToString(userId));

            if (query.exec() == 0) {
                std::print(std::cerr, "Error: Failed to update user balance, no rows affected\n");
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error updating user balance: {}\n", e.what());
        }
    }

    void updateUserHoldings(const boost::uuids::uuid& userId, const boost::uuids::uuid& shareId, std::int64_t amount) {
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, R"(
                INSERT INTO user_holdings (user_id, share_id, quantity)
                VALUES (?, ?, ?)
                ON CONFLICT(user_id, share_id) DO UPDATE SET quantity = quantity + excluded.quantity
            )"};
            query.bind(1, utility::uuidToString(userId));
            query.bind(2, utility::uuidToString(shareId));
            query.bind(3, amount);

            if (query.exec() == 0) {
                std::print(std::cerr, "Error: Failed to update user holdings, no rows affected\n");
            }

            // Delete row if quantity becomes 0
            SQLite::Statement deleteQuery{db, R"(
                DELETE FROM user_holdings 
                WHERE user_id = ? AND share_id = ? AND quantity = 0
            )"};
            deleteQuery.bind(1, utility::uuidToString(userId));
            deleteQuery.bind(2, utility::uuidToString(shareId));
            deleteQuery.exec();
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error updating user holdings: {}\n", e.what());
        }
    }

    void createTransaction(const boost::uuids::uuid& userId, const boost::uuids::uuid& shareId,
                           std::int64_t quantity, std::uint16_t price) {
        const auto transactionId = utility::generateUUID();
        
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, "INSERT INTO transactions (id, user_id, share_id, quantity, price) VALUES (?, ?, ?, ?, ?)"};
            query.bind(1, utility::uuidToString(transactionId));
            query.bind(2, utility::uuidToString(userId));
            query.bind(3, utility::uuidToString(shareId));
            query.bind(4, quantity);
            query.bind(5, price);

            if (query.exec() == 0) {
                std::print(std::cerr, "Error: Failed to create transaction, no rows affected\n");
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error creating transaction: {}\n", e.what());
        }                
    }

    std::vector<TransactionData> getUserTransactions(const boost::uuids::uuid& userId, std::size_t offset, std::size_t limit) {
        std::vector<TransactionData> transactions;
        
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, R"(
                SELECT id, user_id, share_id, quantity, price, timestamp 
                FROM transactions 
                WHERE user_id = ? 
                ORDER BY timestamp DESC 
                LIMIT ? OFFSET ?
            )"};
            
            query.bind(1, utility::uuidToString(userId));
            query.bind(2, static_cast<int>(limit));
            query.bind(3, static_cast<int>(offset));

            while (query.executeStep()) {
                TransactionData transaction{
                    utility::stringToUUID(query.getColumn(0).getText()),
                    utility::stringToUUID(query.getColumn(1).getText()),
                    utility::stringToUUID(query.getColumn(2).getText()),
                    query.getColumn(3).getInt64(),
                    static_cast<std::uint16_t>(query.getColumn(4).getUInt()),
                    query.getColumn(5).getText()
                };
                transactions.push_back(transaction);
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error fetching user transactions: {}\n", e.what());
        }
        
        return transactions;
    }

    void uploadFinishedOrder(const engine::Order& order) {
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, R"(
                INSERT INTO finished_orders 
                (id, user_id, order_type, order_mode, event_id, share_id, quantity, price, filled_quantity, transacted_value, created_at)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            )"};
            query.bind(1, utility::uuidToString(order.id));
            query.bind(2, utility::uuidToString(order.userId));
            query.bind(3, order.type == engine::OrderType::BUY ? "BUY" : "SELL");
            query.bind(4, order.mode == engine::OrderMode::MARKET ? "MARKET" : "LIMIT");
            query.bind(5, utility::uuidToString(order.eventId));
            query.bind(6, utility::uuidToString(order.shareId));
            query.bind(7, static_cast<std::int64_t>(order.quantity));
            query.bind(8, static_cast<std::int64_t>(order.price));
            query.bind(9, static_cast<std::int64_t>(order.quantity - order.leftoverQuantitiy()));
            query.bind(10, static_cast<std::int64_t>(order.totalTransactedValue()));
            query.bind(11, utility::unixToString(order.getCreatedAtUnix()));

            if (query.exec() == 0) {
                std::print(std::cerr, "Error: Failed to upload finished order, no rows affected\n");
            }
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Error uploading finished order: {}\n", e.what());
        }
    }
}