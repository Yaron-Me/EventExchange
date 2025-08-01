#pragma once

#include <string>

namespace database {
    bool registerUser(const std::string& username, const std::string& password);
    bool createStock(const std::string& stockName, const std::string& stockDescription,
                     const std::string& yesShareName, const std::string& noShareName);
    void initializeDatabase(const std::string& filePath);
}