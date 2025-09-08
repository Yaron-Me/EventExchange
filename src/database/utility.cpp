#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <print>
#include <cstdlib>

#include "utility.hpp"

namespace database {
    std::string getDatabasePath() {
        // Check for environment variable first
        const char* envPath = std::getenv("DB_PATH");
        if (envPath) {
            return std::string(envPath);
        }
        // Default to production database
        return "proddb.db3";
    }

    void initializeDatabase() {
        try {
            SQLite::Database db{getDatabasePath(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE};

            // Enable foreign key constraints
            db.exec("PRAGMA foreign_keys = ON;");

            std::ifstream schemaFile{"../src/database/dblayout.sql"};
            if (!schemaFile) {
                std::print(std::cerr, "Error: Could not open {} file\n", "../src/database/dblayout.sql");
                std::remove(getDatabasePath().c_str());
                return;
            }
            
            std::stringstream buffer{};
            buffer << schemaFile.rdbuf();
            const std::string schemaSQL{buffer.str()};
            
            db.exec(schemaSQL);
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Exception: {}", e.what());
        }
    }

    void deleteDatabase() {
        try {
            std::remove(getDatabasePath().c_str());
        }
        catch (const std::exception& e) {
            std::print(std::cerr, "Exception: {}", e.what());
        }
    }
}