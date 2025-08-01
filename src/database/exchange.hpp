#pragma once

#include <string>

namespace database {
    bool createStock(const std::string& stockName, const std::string& stockDescription,
                     const std::string& yesShareName, const std::string& noShareName);
}