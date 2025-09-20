#include <catch2/catch_test_macros.hpp>
#include <print>

#include "../src/engine/orderqueue.hpp"

#include "../src/utility/uuid.hpp"

#include "../src/database/utility.hpp"
#include "../src/database/user.hpp"
#include "../src/database/event.hpp"

using namespace utility;
using namespace engine;
using namespace database;

TEST_CASE("OrderQueue") {
    deleteDatabase();
    initializeDatabase();

    const auto [success, userId1]{registerUser("testuser", "password")};
    const auto [success2, userId2]{registerUser("testuser2", "password")};
    const auto [eventCreated, eventData]{createEvent("Test Event", "This is a test event", "Yes Share", "No Share")};

    updateUserBalance(userId1, 600 * 50);
    updateUserHoldings(userId2, eventData.yesShare.id, 600);

    OrderQueue orderQueue;

    auto order1{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                                         eventData.id, eventData.yesShare.id, 100, 50)};
    auto order2{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                                         eventData.id, eventData.yesShare.id, 200, 50)};
    auto order3{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                                         eventData.id, eventData.yesShare.id, 300, 50)};

    orderQueue.addOrder(order1);
    REQUIRE(orderQueue.getTotalQuantity() == 100);

    orderQueue.addOrder(order2);
    REQUIRE(orderQueue.getTotalQuantity() == 300);

    orderQueue.addOrder(order3);
    REQUIRE(orderQueue.getTotalQuantity() == 600);

    {
        auto order{std::make_shared<Order>(userId2, OrderType::SELL, OrderMode::MARKET,
                     eventData.id, eventData.yesShare.id, 250, 50)};

        auto filled{orderQueue.fillOrder(order)};
        
        REQUIRE(filled == 250);
        REQUIRE(order->leftoverQuantitiy() == 0);
        REQUIRE(orderQueue.getTotalQuantity() == 350);
        REQUIRE(order1->leftoverQuantitiy() == 0);
        REQUIRE(order2->leftoverQuantitiy() == 50);
        REQUIRE(order3->leftoverQuantitiy() == 300);
        REQUIRE(orderQueue.getOrderCount() == 2);

        auto transactions1{getUserTransactions(userId1, 0, 2)};
        REQUIRE(transactions1.size() == 2);
        REQUIRE(transactions1[0].amount == 100);
        REQUIRE(transactions1[0].price == 50);
        REQUIRE(transactions1[1].amount == 150);
        REQUIRE(transactions1[1].price == 50);
        
        auto transactions2{getUserTransactions(userId2, 0, 1)};
        REQUIRE(transactions2.size() == 1);
        REQUIRE(transactions2[0].amount == -250);
        REQUIRE(transactions2[0].price == 50);
    }

    {
        orderQueue.cancelOrder(order2);

        REQUIRE(orderQueue.getTotalQuantity() == 300);
        REQUIRE(order2->leftoverQuantitiy() == 50);
        REQUIRE(order3->leftoverQuantitiy() == 300);
        REQUIRE(orderQueue.getOrderCount() == 1);
    }

    {
        auto order{std::make_shared<Order>(userId2, OrderType::SELL, OrderMode::MARKET,
                     eventData.id, eventData.yesShare.id, 500, 50)};

        auto filled{orderQueue.fillOrder(order)};

        REQUIRE(filled == 300);
        REQUIRE(order->leftoverQuantitiy() == 200);
        REQUIRE(orderQueue.getTotalQuantity() == 0);
        REQUIRE(order3->leftoverQuantitiy() == 0);
        REQUIRE(orderQueue.getOrderCount() == 0);

        const auto transactions1{getUserTransactions(userId1, 0, 3)};
        REQUIRE(transactions1.size() == 3);
        REQUIRE(transactions1[2].amount == 300);
        REQUIRE(transactions1[2].price == 50);

        auto transactions2{getUserTransactions(userId2, 0, 2)};
        REQUIRE(transactions2.size() == 2);
        REQUIRE(transactions2[1].amount == -300);
        REQUIRE(transactions2[1].price == 50);
    }

    REQUIRE(getUserBalance(userId1) == 600 * 50 - 600 * 50 + 50 * 50);
    REQUIRE(getUserBalance(userId2) == 600 * 50 - 50 * 50);

    REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 550);
    REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 50);
}