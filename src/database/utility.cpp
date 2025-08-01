#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "utility.hpp"

namespace database {
    void initializeDatabase(const std::string& filePath) {
        try {
            SQLite::Database db{"exchange.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE};

            std::ifstream schemaFile{filePath};
            if (!schemaFile) {
                std::cerr << "Error: Could not open " << filePath << " file\n";
                std::remove("exchange.db3");
                return;
            }
            
            std::stringstream buffer{};
            buffer << schemaFile.rdbuf();
            const std::string schemaSQL{buffer.str()};
            
            db.exec(schemaSQL);
        }
        catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << '\n';
        }
    }
}