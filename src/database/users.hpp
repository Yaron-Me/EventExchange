#pragma once

#include <string>

namespace database {
    bool registerUser(const std::string& username, const std::string& password);
}