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
            std::cerr << "Error registering user: " << e.what() << std::endl;
            return false;
        }
    }
}