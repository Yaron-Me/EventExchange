#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>

#include "users.hpp"
#include "../utility/uuid.hpp"

namespace database {
    bool registerUser(const std::string& username, const std::string& password) {
        const auto userId{utility::generateUUID()};
        const std::string userIdStr{utility::uuidToString(userId)};

        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READWRITE};
            
            SQLite::Statement query{db, "INSERT INTO users (id, name, password, balance) VALUES (?, ?, ?, ?)"};
            query.bind(1, userIdStr);
            query.bind(2, username);
            query.bind(3, password);
            query.bind(4, 0);
            
            if (query.exec() > 0) {
                return true;
            } else {
                return false;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error registering user: " << e.what() << "\n";
            return false;
        }
    }

    std::int64_t getUserBalance(const boost::uuids::uuid& userId) {
        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READONLY};

            SQLite::Statement query{db, "SELECT balance FROM users WHERE id = ?"};
            query.bind(1, utility::uuidToString(userId));

            if (query.executeStep()) {
                return query.getColumn(0).getInt64();
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error fetching user balance: " << e.what() << "\n";
        }

        return 0;
    }

    std::map<boost::uuids::uuid, std::uint32_t> getUserHoldings(const boost::uuids::uuid& userId) {
        std::map<boost::uuids::uuid, std::uint32_t> holdings;
        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READONLY};

            SQLite::Statement query{db, "SELECT share_id, quantity FROM user_holdings WHERE user_id = ?"};
            query.bind(1, utility::uuidToString(userId));

            while (query.executeStep()) {
                auto shareId = utility::stringToUUID(query.getColumn(0).getText());
                auto quantity = query.getColumn(1).getUInt();
                holdings[shareId] = quantity;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error fetching user holdings: " << e.what() << "\n";
        }
        return holdings;
    }

    void increaseUserBalance(const boost::uuids::uuid& userId, std::int64_t amount) {
        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READWRITE};

            SQLite::Statement query{db, "UPDATE users SET balance = balance + ? WHERE id = ?"};
            query.bind(1, amount);
            query.bind(2, utility::uuidToString(userId));

            if (query.exec() == 0) {
                std::cerr << "No rows updated for user ID: " << utility::uuidToString(userId) << "\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error increasing user balance: " << e.what() << "\n";
        }
    }
}