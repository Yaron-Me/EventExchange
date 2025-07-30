#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <SQLiteCpp/SQLiteCpp.h>

#include "utility/UUID.hpp"
#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"

int main() {
    try {
        // Initialize SQLite database
        SQLite::Database db("exchange.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
        
        std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";
        
        // Read and execute database schema from dblayout.txt
        std::cout << "Reading database schema from dblayout.txt...\n";
        
        std::ifstream schemaFile("dblayout.txt");
        if (!schemaFile.is_open()) {
            std::cerr << "Error: Could not open dblayout.txt file\n";
            return -1;
        }
        
        std::stringstream buffer;
        buffer << schemaFile.rdbuf();
        std::string schemaSQL = buffer.str();
        schemaFile.close();
        
        std::cout << "Executing database schema...\n";
        db.exec(schemaSQL);
        
        std::cout << "Database schema created successfully!\n";
        
        // Initialize your exchange
        exchange::Exchange exchange;
        
        return 0;
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return -1;
    }
}