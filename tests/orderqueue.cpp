#include <catch2/catch_test_macros.hpp>

#include "../src/exchange/orderqueue.hpp"

#include "../src/utility/uuid.hpp"

#include "../src/database/utility.hpp"
#include "../src/database/user.hpp"
#include "../src/database/exchange.hpp"

using namespace utility;
using namespace exchange;
using namespace database;

TEST_CASE("Order Queue") {
    deleteDatabase();
    initializeDatabase();

    const auto [success, userId] = registerUser("testuser", "password");
    const auto [eventCreated, eventData] = createEvent("Test Event", "This is a test event", "Yes Share", "No Share");

    updateUserBalance(userId, 5000);

    OrderQueue orderQueue;

    auto order1 = std::make_shared<Order>(userId, OrderType::BUY, OrderMode::MARKET,
                                         eventData.id, eventData.yesShare.id, 100, 50);
    auto order2 = std::make_shared<Order>(userId, OrderType::BUY, OrderMode::MARKET,
                                         eventData.id, eventData.yesShare.id, 200, 50);
    auto order3 = std::make_shared<Order>(userId, OrderType::BUY, OrderMode::MARKET,
                                         eventData.id, eventData.yesShare.id, 300, 50);

    orderQueue.addOrder(order1);
    REQUIRE(orderQueue.getTotalQuantity() == 100);

    orderQueue.addOrder(order2);
    REQUIRE(orderQueue.getTotalQuantity() == 300);

    orderQueue.addOrder(order3);
    REQUIRE(orderQueue.getTotalQuantity() == 600);

    {
        const auto filled = orderQueue.fill(250, 50);
        REQUIRE(filled == 250);
        REQUIRE(orderQueue.getTotalQuantity() == 350);
        REQUIRE(order1->leftoverQuantitiy() == 0);
        REQUIRE(order2->leftoverQuantitiy() == 50);
        REQUIRE(order3->leftoverQuantitiy() == 300);

        REQUIRE(orderQueue.getOrderCount() == 2);
    }

    {
        const auto filled = orderQueue.fill(200, 50);
        REQUIRE(filled == 200);
        REQUIRE(orderQueue.getTotalQuantity() == 150);
        REQUIRE(order1->leftoverQuantitiy() == 0);
        REQUIRE(order2->leftoverQuantitiy() == 0);
        REQUIRE(order3->leftoverQuantitiy() == 150);
        REQUIRE(orderQueue.getOrderCount() == 1);
    }
    
    {
        orderQueue.cancelOrder(order3);
        REQUIRE(orderQueue.getTotalQuantity() == 0);
        REQUIRE(order1->leftoverQuantitiy() == 0);
        REQUIRE(order2->leftoverQuantitiy() == 0);
        REQUIRE(order3->leftoverQuantitiy() == 150);
        REQUIRE(orderQueue.getOrderCount() == 0);
    }
}