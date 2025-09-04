#include <catch2/catch_test_macros.hpp>
#include "../src/exchange/Order.hpp"
#include "../src/utility/uuid.hpp"

using namespace utility;
using namespace exchange;

TEST_CASE("BUY Order test") {
    Order order{generateUUID(), OrderType::BUY, OrderMode::MARKET,
                generateUUID(), generateUUID(), 100, 50};

    REQUIRE(order.leftoverQuantitiy() == 100);
    REQUIRE(order.positionValue() == 5000);

    const auto filled = order.fill(5, 40);

    REQUIRE(order.leftoverQuantitiy() == 95);
    REQUIRE(order.positionValue() == 5000 - 250);
    REQUIRE(filled == 5);
    REQUIRE(order.totalTransactedMoney() == 5 * 40);

    const auto filled2 = order.fill(5, 51);

    REQUIRE(order.leftoverQuantitiy() == 95);
    REQUIRE(order.positionValue() == 5000 - 250);
    REQUIRE(filled2 == 0);
    REQUIRE(order.totalTransactedMoney() == 5 * 40);

    const auto filled3 = order.fill(96, 50);

    REQUIRE(order.leftoverQuantitiy() == 0);
    REQUIRE(order.positionValue() == 0);
    REQUIRE(filled3 == 95);
    REQUIRE(order.totalTransactedMoney() == 5*40 + 95*50);
}

TEST_CASE("SELL Order test") {
    Order order{generateUUID(), OrderType::SELL, OrderMode::MARKET,
                generateUUID(), generateUUID(), 100, 50};

    REQUIRE(order.leftoverQuantitiy() == 100);
    REQUIRE(order.positionValue() == 5000);

    const auto filled = order.fill(5, 40);

    REQUIRE(order.leftoverQuantitiy() == 100);
    REQUIRE(order.positionValue() == 5000);
    REQUIRE(filled == 0);
    REQUIRE(order.totalTransactedMoney() == 0);

    const auto filled2 = order.fill(5, 51);

    REQUIRE(order.leftoverQuantitiy() == 95);
    REQUIRE(order.positionValue() == 5000 - 5*50);
    REQUIRE(filled2 == 5);
    REQUIRE(order.totalTransactedMoney() == 5*51);

    const auto filled3 = order.fill(96, 50);

    REQUIRE(order.leftoverQuantitiy() == 0);
    REQUIRE(order.positionValue() == 0);
    REQUIRE(filled3 == 95);
    REQUIRE(order.totalTransactedMoney() == 5*51 + 95*50);
}