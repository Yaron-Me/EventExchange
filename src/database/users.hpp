#pragma once

#include <string>

namespace database {
    bool registerUser(const std::string& username, const std::string& password);
    std::int64_t getUserBalance(const std::string& userId);
}