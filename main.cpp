#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>
#include <SQLiteCpp/SQLiteCpp.h>

#include "utility/UUID.hpp"
#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"

int main() {
    try {
        // Initialize SQLite database
        SQLite::Database db("exchange.db3", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
        
        std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";
        
        // Create a simple table for testing
        db.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT)");
        
        // Insert some test data
        SQLite::Statement query(db, "INSERT INTO test (name) VALUES (?)");
        query.bind(1, "Test Entry");
        query.exec();
        
        std::cout << "SQLiteCpp is working correctly!\n";
        
        // Initialize your exchange
        exchange::Exchange exchange;
        
        return 0;
    }
    catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
        return -1;
    }
}