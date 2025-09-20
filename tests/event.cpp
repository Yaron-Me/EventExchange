#include <catch2/catch_test_macros.hpp>

#include "../src/engine/event.hpp"

#include "../src/utility/uuid.hpp"

#include "../src/database/utility.hpp"
#include "../src/database/user.hpp"
#include "../src/database/event.hpp"

using namespace utility;
using namespace engine;
using namespace database;

TEST_CASE("Share buying and issuing new shares") {
    deleteDatabase();
    initializeDatabase();

    const auto [success, userId1]{registerUser("testuser", "password")};
    const auto [success2, userId2]{registerUser("testuser2", "password")};
    const auto [eventCreated, eventData]{createEvent("Test Event", "This is a test event", "Yes Share", "No Share")};

    uint64_t initialBalance{100000000};

    updateUserBalance(userId1, initialBalance);
    updateUserBalance(userId2, initialBalance);
    updateUserHoldings(userId1, eventData.noShare.id, 1000);
    updateUserHoldings(userId1, eventData.yesShare.id, 1000);
    updateUserHoldings(userId2, eventData.noShare.id, 1000);
    updateUserHoldings(userId2, eventData.yesShare.id, 1000);


    Event event{eventData.id, eventData.yesShare.id, eventData.noShare.id};

    for (int price{450}; price < 550; price += 10) {
        auto order{std::make_shared<Order>(userId2, OrderType::SELL, OrderMode::LIMIT,
                         eventData.id, eventData.yesShare.id, 100, price)};
        event.addOrder(order);
    }

    for (int price{450}; price < 550; price += 10) {
        auto order{std::make_shared<Order>(userId2, OrderType::BUY, OrderMode::LIMIT,
                         eventData.id, eventData.noShare.id, 100, price)};
        event.addOrder(order);
    }

    event.printOrderbook();

    {
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 305, 500)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 0);

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 800});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{895, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460) * 100 - 470 * 5);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 1000 + 305);

        REQUIRE(getUserBalance(userId2) == initialBalance + (450 + 460 - 540) * 100 - 530 * 5);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 1000 - 200);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == 1000 + 105);

        REQUIRE(getEvent(eventData.id).issued == 105);
    }

    {
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 10000, 500)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 10000 - 795);

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{10000 - 795, 400});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{500, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460 + 470 + 470 + 480 + 480 + 490 + 490 + 500 + 500) * 100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 1000 + 300 + 800);

        REQUIRE(getUserBalance(userId2) == initialBalance + (450 + 460 + 470 + 480 + 490 + 500 - 540 - 530 - 520 - 510 - 500) * 100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 1000 - 200 - 400);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == 1000 + 100 + 400);

        REQUIRE(getEvent(eventData.id).issued == 500);

        event.cancelOrder(order);
        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 400});
    }

    {
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::MARKET,
                            eventData.id, eventData.yesShare.id, 1000, 550)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 100);

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 0});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{0, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460 + 470 + 470 + 480 + 480 + 490 + 490 + 500 + 500 + 510 + 510 + 520 + 520 + 530 + 530 + 540 + 540 + 550) * 100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 1000 + 300 + 800 + 900);

        REQUIRE(getUserBalance(userId2) == initialBalance);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 1000 - 200 - 400 - 400);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == 1000 + 100 + 400 + 500);

        REQUIRE(getEvent(eventData.id).issued == 1000);
    }
}