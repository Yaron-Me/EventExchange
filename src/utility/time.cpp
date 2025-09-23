#include <chrono>

namespace utility {
    std::string unixToString(std::time_t unixTime) {
        std::tm* gmt = std::gmtime(&unixTime);
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", gmt);
        return std::string(buffer);
    }
}