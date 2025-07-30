#include "database.hpp"

#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>
#include <sstream>

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