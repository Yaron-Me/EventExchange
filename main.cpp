#include <boost/uuid.hpp>
#include <iostream>

#include "utility/UUID.hpp"
#include "exchange/Order.hpp"
#include "exchange/OrderQueue.hpp"
#include "exchange/User.hpp"

int main() {
    // Example usage
    boost::uuids::uuid newUUID = utility::generateUUID();
    std::cout << "Generated UUID: " << newUUID << std::endl;
}