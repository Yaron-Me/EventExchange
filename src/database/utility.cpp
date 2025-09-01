#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <print>

#include "utility.hpp"

namespace database {
    void initializeDatabase(const std::string& filePath) {
        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE};

            std::ifstream schemaFile{filePath};
            if (!schemaFile) {
                std::print(std::cerr, "Error: Could not open {} file\n", filePath);
                std::remove("exchange.db3");
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
}