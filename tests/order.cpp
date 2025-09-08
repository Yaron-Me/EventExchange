#include <catch2/catch_test_macros.hpp>

#include "../src/exchange/Order.hpp"

#include "../src/utility/uuid.hpp"

#include "../src/database/utility.hpp"
#include "../src/database/users.hpp"
#include "../src/database/exchange.hpp"

using namespace utility;
using namespace exchange;

TEST_CASE("Buy Order") {
    database::deleteDatabase();
    database::initializeDatabase();

    const auto [success, userId] = database::registerUser("testuser", "password");
    const auto [eventCreated, eventData] = database::createEvent("Test Event", "This is a test event", "Yes Share", "No Share");

    database::updateUserBalance(userId, 5000);

    Order order{userId, OrderType::BUY, OrderMode::MARKET,
                eventData.id, eventData.yesShare.id, 100, 50};

    REQUIRE(order.leftoverQuantitiy() == 100);
    REQUIRE(order.positionValue() == 5000);

    {
        const auto filled = order.fill(5, 40);

        REQUIRE(order.leftoverQuantitiy() == 95);
        REQUIRE(order.positionValue() == 5000 - 250);
        REQUIRE(filled == 5);
        REQUIRE(order.totalTransactedValue() == 5 * 40);

        const auto userBalance = database::getUserBalance(userId);
        REQUIRE(userBalance == 5000 - order.totalTransactedValue());

        const auto transactions = database::getUserTransactions(userId, 0, 1);
        REQUIRE(transactions[0].amount == filled);
        REQUIRE(transactions[0].price == 40);
        REQUIRE(transactions[0].shareId == eventData.yesShare.id);
        REQUIRE(transactions[0].userId == userId);
    }
    {
        const auto filled = order.fill(5, 51);

        REQUIRE(order.leftoverQuantitiy() == 95);
        REQUIRE(order.positionValue() == 5000 - 250);
        REQUIRE(filled == 0);
        REQUIRE(order.totalTransactedValue() == 5 * 40);

        const auto userBalance = database::getUserBalance(userId);
        REQUIRE(userBalance == 5000 - order.totalTransactedValue());

        const auto transactions = database::getUserTransactions(userId, 0, 2);
        REQUIRE(transactions.size() == 1);
    }
    {
        const auto filled = order.fill(96, 50);

        REQUIRE(order.leftoverQuantitiy() == 0);
        REQUIRE(order.positionValue() == 0);
        REQUIRE(filled == 95);
        REQUIRE(order.totalTransactedValue() == 5*40 + 95*50);

        const auto userBalance = database::getUserBalance(userId);
        REQUIRE(userBalance == 5000 - order.totalTransactedValue());

        const auto transactions = database::getUserTransactions(userId, 1, 3);
        REQUIRE(transactions.size() == 1);
        REQUIRE(transactions[0].amount == filled);
        REQUIRE(transactions[0].price == 50);
        REQUIRE(transactions[0].shareId == eventData.yesShare.id);
        REQUIRE(transactions[0].userId == userId);
    }
}

TEST_CASE("Sell order") {
    database::deleteDatabase();
    database::initializeDatabase();

    const auto [success, userId] = database::registerUser("testuser", "password");
    const auto [eventCreated, eventData] = database::createEvent("Test Event", "This is a test event", "Yes Share", "No Share");

    database::updateUserHoldings(userId, eventData.yesShare.id, 100);

    Order order{userId, OrderType::SELL, OrderMode::MARKET,
                eventData.id, eventData.yesShare.id, 100, 50};

    REQUIRE(order.leftoverQuantitiy() == 100);
    REQUIRE(order.positionValue() == 5000);

    {
        const auto filled = order.fill(5, 40);

        REQUIRE(order.leftoverQuantitiy() == 100);
        REQUIRE(order.positionValue() == 5000);
        REQUIRE(filled == 0);
        REQUIRE(order.totalTransactedValue() == 0);

        const auto userBalance = database::getUserBalance(userId);
        REQUIRE(userBalance == 0 + order.totalTransactedValue());

        const auto transactions = database::getUserTransactions(userId, 0, 1);
        REQUIRE(transactions.size() == 0);
    }
    {
        const auto filled = order.fill(5, 51);

        REQUIRE(order.leftoverQuantitiy() == 95);
        REQUIRE(order.positionValue() == 5000 - 5*50);
        REQUIRE(filled == 5);
        REQUIRE(order.totalTransactedValue() == 5*51);

        const auto userBalance = database::getUserBalance(userId);
        REQUIRE(userBalance == 0 + order.totalTransactedValue());

        const auto transactions = database::getUserTransactions(userId, 0, 2);
        REQUIRE(transactions[0].amount == -filled);
        REQUIRE(transactions[0].price == 51);
        REQUIRE(transactions[0].shareId == eventData.yesShare.id);
        REQUIRE(transactions[0].userId == userId);
    }
    {
        const auto filled = order.fill(96, 50);

        REQUIRE(order.leftoverQuantitiy() == 0);
        REQUIRE(order.positionValue() == 0);
        REQUIRE(filled == 95);
        REQUIRE(order.totalTransactedValue() == 5*51 + 95*50);

        const auto userBalance = database::getUserBalance(userId);
        REQUIRE(userBalance == 0 + order.totalTransactedValue());

        const auto transactions = database::getUserTransactions(userId, 1, 3);
        REQUIRE(transactions.size() == 1);
        REQUIRE(transactions[0].amount == -filled);
        REQUIRE(transactions[0].price == 50);
        REQUIRE(transactions[0].shareId == eventData.yesShare.id);
        REQUIRE(transactions[0].userId == userId);
    }
}