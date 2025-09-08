#pragma once

#include <string>

namespace database {
    void initializeDatabase();
    void deleteDatabase();
    std::string getDatabasePath();
}