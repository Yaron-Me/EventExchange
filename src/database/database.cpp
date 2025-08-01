#include "database.hpp"

#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../utility/UUID.hpp"

namespace database {

    bool registerUser(const std::string& username, const std::string& password) {
        // Generate a random UUID for the user
        boost::uuids::uuid userId = utility::generateUUID();
        std::string userIdStr = utility::uuidToString(userId);

        try {
            // Open the database
            SQLite::Database db("exchange.db3", SQLite::OPEN_READWRITE);
            
            // Create the user with balance = 0
            SQLite::Statement query(db, "INSERT INTO users (id, name, password, balance) VALUES (?, ?, ?, ?)");
            query.bind(1, userIdStr);
            query.bind(2, username);
            query.bind(3, password);
            query.bind(4, 0); // balance starts at 0
            
            int result = query.exec();
            
            if (result > 0) {
                std::cout << "User '" << username << "' registered successfully with ID: " << userIdStr << std::endl;
                return true;
            } else {
                std::cerr << "Failed to register user '" << username << "'" << std::endl;
                return false;
            }
            
        } catch (std::exception& e) {
            std::cerr << "Error registering user: " << e.what() << std::endl;
            return false;
        }
    }

    bool createStock(const std::string& stockName, const std::string& stockDescription,
                     const std::string& yesShareName, const std::string& noShareName) {
        boost::uuids::uuid stockId = utility::generateUUID();
        boost::uuids::uuid yesShareId = utility::generateUUID();
        boost::uuids::uuid noShareId = utility::generateUUID();

        std::string stockIdStr = utility::uuidToString(stockId);
        std::string yesShareIdStr = utility::uuidToString(yesShareId);
        std::string noShareIdStr = utility::uuidToString(noShareId);

        try {
            // Open the database
            SQLite::Database db("exchange.db3", SQLite::OPEN_READWRITE);
            
            // Start a transaction to ensure data consistency
            db.exec("BEGIN TRANSACTION");
            
            // Step 1: Create the stock first (without share references initially)
            // We'll need to update the stock later with the share IDs
            SQLite::Statement stockQuery(db, "INSERT INTO stocks (id, name, description, yes_share_id, no_share_id) VALUES (?, ?, ?, ?, ?)");
            stockQuery.bind(1, stockIdStr);
            stockQuery.bind(2, stockName);
            stockQuery.bind(3, stockDescription);
            stockQuery.bind(4, yesShareIdStr);  // We know the IDs we'll create
            stockQuery.bind(5, noShareIdStr);
            stockQuery.exec();
            
            // Step 2: Create the YES share
            SQLite::Statement yesShareQuery(db, "INSERT INTO shares (id, name, stock_id) VALUES (?, ?, ?)");
            yesShareQuery.bind(1, yesShareIdStr);
            yesShareQuery.bind(2, yesShareName);
            yesShareQuery.bind(3, stockIdStr);
            yesShareQuery.exec();
            
            // Step 3: Create the NO share
            SQLite::Statement noShareQuery(db, "INSERT INTO shares (id, name, stock_id) VALUES (?, ?, ?)");
            noShareQuery.bind(1, noShareIdStr);
            noShareQuery.bind(2, noShareName);
            noShareQuery.bind(3, stockIdStr);
            noShareQuery.exec();
            
            // Commit the transaction
            db.exec("COMMIT");
            
            std::cout << "Stock '" << stockName << "' created successfully with ID: " << stockIdStr << std::endl;
            std::cout << "  Yes share '" << yesShareName << "' ID: " << yesShareIdStr << std::endl;
            std::cout << "  No share '" << noShareName << "' ID: " << noShareIdStr << std::endl;
            
            return true;
            
        } catch (std::exception& e) {
            // Rollback on error
            try {
                SQLite::Database db("exchange.db3", SQLite::OPEN_READWRITE);
                db.exec("ROLLBACK");
            } catch (...) {
                // Ignore rollback errors
            }
            std::cerr << "Error creating stock: " << e.what() << std::endl;
            return false;
        }
    }

    void initializeDatabase(const std::string& filePath) {
        try {
            // Initialize SQLite database
            SQLite::Database db("exchange.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
            
            std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";
            
            // Read and execute database schema from dblayout.txt
            std::cout << "Reading database schema from " << filePath << "...\n";

            std::ifstream schemaFile(filePath);
            if (!schemaFile.is_open()) {
                std::cerr << "Error: Could not open " << filePath << " file\n";
                // if file wasn't found delete the database file
                std::remove("exchange.db3");
                return;
            }
            
            std::stringstream buffer;
            buffer << schemaFile.rdbuf();
            std::string schemaSQL = buffer.str();
            schemaFile.close();
            
            std::cout << "Executing database schema...\n";
            db.exec(schemaSQL);
            
            std::cout << "Database schema created successfully!\n";

            return;
        }
        catch (std::exception& e) {
            std::cout << "Exception: " << e.what() << std::endl;
            return;
        }
    }

}