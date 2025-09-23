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
    uint64_t initialHoldings{2000};

    updateUserBalance(userId1, initialBalance);
    updateUserBalance(userId2, initialBalance);
    updateUserHoldings(userId1, eventData.noShare.id, initialHoldings);
    updateUserHoldings(userId1, eventData.yesShare.id, initialHoldings);
    updateUserHoldings(userId2, eventData.noShare.id, initialHoldings);
    updateUserHoldings(userId2, eventData.yesShare.id, initialHoldings);

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

    { // test tie breaker based on quantity, picks buy
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 305, 500)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 0);

        // We can buy 100 for 450 and 460, then we can match 100 for 460, and then we can buy 5 at 470

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 795});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{900, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460) * 100 - 470 * 5);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings + 305);

        // A match for this user means they pay (1000 - p)
        REQUIRE(getUserBalance(userId2) == initialBalance + (450 + 460 - 540) * 100 + 470 * 5);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings - 205);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings + 100);

        REQUIRE(getEvent(eventData.id).issued == 100);
    }

    { // test tie breaker based on quantity, picks match
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 100, 500)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 0);

        // We match 100 for 470, and don't buy, because cost is the same buy quality is higher

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 795});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{800, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460) * 100 - 470 * 105);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings + 305 + 100);

        REQUIRE(getUserBalance(userId2) == initialBalance + (450 + 460 - 540 - 530) * 100 + 470 * 5);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings - 205);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings + 100 + 100);
    }

    { // test large buy order that matches multiple levels and issues new shares but stops and doesn't fully fill or get added
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 10000, 500)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 10000 - 695);

        // We can simply buy and match everything up to 500, and because it's a limit order, it gets added to the book

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{10000 - 695, 400});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{500, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460 + 470 + 470 + 480 + 480 + 490 + 490 + 500 + 500) * 100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings + 300 + 800);

        REQUIRE(getUserBalance(userId2) == initialBalance + (450 + 460 + 470 + 480 + 490 + 500 - 540 - 530 - 520 - 510 - 500) * 100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings - 200 - 400);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings + 100 + 400);

        REQUIRE(getEvent(eventData.id).issued == 500);

        event.cancelOrder(order);
        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 400});
    }

    { // test market order that matches multiple levels and issues new shares but stops and doesn't fully fill or get added 
        auto order{std::make_shared<Order>(userId1, OrderType::BUY, OrderMode::MARKET,
                            eventData.id, eventData.yesShare.id, 1000, 550)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 100);

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 0});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{0, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance - (450 + 460 + 460 + 470 + 470 + 480 + 480 + 490 + 490 + 500 + 500 + 510 + 510 + 520 + 520 + 530 + 530 + 540 + 540 + 550) * 100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings + 300 + 800 + 900);

        REQUIRE(getUserBalance(userId2) == initialBalance);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings - 200 - 400 - 400);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings + 100 + 400 + 500);

        REQUIRE(getEvent(eventData.id).issued == 1000);
    }
}

TEST_CASE("Share selling and deleting shares") {
    deleteDatabase();
    initializeDatabase();

    const auto [success, userId1]{registerUser("testuser", "password")};
    const auto [success2, userId2]{registerUser("testuser2", "password")};
    const auto [eventCreated, eventData]{createEvent("Test Event", "This is a test event", "Yes Share", "No Share")};

    uint64_t initialBalance{100000000};
    uint64_t initialHoldings{2000};

    updateIssuedShares(eventData.id, initialHoldings);

    updateUserBalance(userId1, initialBalance);
    updateUserBalance(userId2, initialBalance);
    updateUserHoldings(userId1, eventData.noShare.id, initialHoldings);
    updateUserHoldings(userId1, eventData.yesShare.id, initialHoldings);
    updateUserHoldings(userId2, eventData.noShare.id, initialHoldings);
    updateUserHoldings(userId2, eventData.yesShare.id, initialHoldings);

    Event event{eventData.id, eventData.yesShare.id, eventData.noShare.id};

    for (int price{450}; price < 550; price += 10) {
        auto order{std::make_shared<Order>(userId2, OrderType::BUY, OrderMode::LIMIT,
                         eventData.id, eventData.yesShare.id, 100, price)};
        event.addOrder(order);
    }

    for (int price{450}; price < 550; price += 10) {
        auto order{std::make_shared<Order>(userId2, OrderType::SELL, OrderMode::LIMIT,
                         eventData.id, eventData.noShare.id, 100, price)};
        event.addOrder(order);
    }

    { // test tie breaker based on quantity, picks sell
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 305, 500)};
        event.addOrder(order);

        // We can match 100 for 550, then sell 100 for 540, then match 100 for 540, and then sell 5 at 530

        REQUIRE(order->leftoverQuantitiy() == 0);
        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{895, 0});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{0, 800});

        REQUIRE(getUserBalance(userId1) == initialBalance + (550 + 540 + 540) * 100 + 530 * 5);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings - 305);

        // A match for this user means they gain (1000 - p)
        REQUIRE(getUserBalance(userId2) == initialBalance + (450 - 540 + 460) * 100 - 530 * 5);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings + 105);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings - 200);

        REQUIRE(getEvent(eventData.id).issued == initialHoldings - 200);
    }

    { // test tie breaker based on quantity, picks match
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 100, 500)};
        event.addOrder(order);

        // We match 100 for 530, because cost is the same and match quality is higher

        REQUIRE(order->leftoverQuantitiy() == 0);
        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{895, 0});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{0, 700});

        REQUIRE(getUserBalance(userId1) == initialBalance + (550 + 540 + 540 + 530) * 100 + 530 * 5);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings - 305 - 100);

        // A match for this user means they gain (1000 - p)
        REQUIRE(getUserBalance(userId2) == initialBalance + (450 - 540 + 460 + 470) * 100 - 530 * 5);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings + 105);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings - 200 - 100);

        REQUIRE(getEvent(eventData.id).issued == initialHoldings - 200 - 100);
    }

    { // test large sell order that matches multiple levels and deletes shares but stops and doesn't fully fill or get added
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::LIMIT,
                            eventData.id, eventData.yesShare.id, 10000, 500)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 10000 - 395 - 300);

        // We can simply sell and match everything up to 500, and because it's a limit order, it gets added to the book

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{500, 10000 - 395 - 300});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{0, 400});

        REQUIRE(getUserBalance(userId1) == initialBalance + (550 + 540 + 540 + 530 + 530 + 520 + 520 + 510 + 510 + 500 + 500) * 100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == initialHoldings - 305 - 100 - 395 - 300);

        REQUIRE(getUserBalance(userId2) == initialBalance + (450 + 460 + 470 + 480 + 490 + 500 - 540 - 530 - 520 - 510 - 500) * 100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings + 500);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings - 200 - 100 - 300);

        REQUIRE(getEvent(eventData.id).issued == initialHoldings - 200 - 100 - 300);

        event.cancelOrder(order);
        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{500, 0});
    }

    {
        // test market order that matches multiple levels and deletes shares but stops and doesn't fully fill or get added
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::MARKET,
                            eventData.id, eventData.yesShare.id, 1000, 450)};
        event.addOrder(order);

        REQUIRE(order->leftoverQuantitiy() == 100);

        REQUIRE(event.getBuyAndSellQuantities(eventData.yesShare.id) == std::pair<uint64_t, uint64_t>{0, 0});
        REQUIRE(event.getBuyAndSellQuantities(eventData.noShare.id) == std::pair<uint64_t, uint64_t>{0, 0});

        REQUIRE(getUserBalance(userId1) == initialBalance + (550 + 540 + 540 + 530 + 530 + 520 + 520 + 510 + 510 + 500 + 500 + 490 + 490 + 480 + 480 + 470 + 470 + 460 + 460 + 450) * 100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 0);

        REQUIRE(getUserBalance(userId2) == initialBalance);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == initialHoldings + 500 + 500);
        REQUIRE(getUserShareHoldings(userId2, eventData.noShare.id) == initialHoldings - 200 - 100 - 300 - 400);

        REQUIRE(getEvent(eventData.id).issued == initialHoldings - 200 - 100 - 300 - 400);
    }

}