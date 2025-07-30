#include <boost/uuid.hpp>
#include <crow.h>
#include <iostream>

#include "utility/UUID.hpp"
#include "exchange/Order.hpp"
#include "exchange/Exchange.hpp"
#include "database/database.hpp"

int main() {

    initializeDatabase("/home/soul/EventExchange/src/database/dblayout.txt");

    return 0;
}