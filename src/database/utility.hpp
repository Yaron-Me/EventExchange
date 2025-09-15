#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <memory>

namespace database {
    void initializeDatabase();
    void deleteDatabase();
    std::string getDatabasePath();
    SQLite::Database& getDatabase();
    std::unique_ptr<SQLite::Database>& getDatabaseInstance();
}