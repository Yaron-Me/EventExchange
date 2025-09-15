#include <catch2/catch_test_macros.hpp>
#include <print>

#include "../src/engine/orderqueue.hpp"
#include "../src/engine/share.hpp"

#include "../src/utility/uuid.hpp"

#include "../src/database/utility.hpp"
#include "../src/database/user.hpp"
#include "../src/database/exchange.hpp"

using namespace utility;
using namespace engine;
using namespace database;

TEST_CASE("Share Buy fills Sell") {
    deleteDatabase();
    initializeDatabase();

    const auto [success1, userId1]{registerUser("testuser", "password")};
    const auto [success2, userId2]{registerUser("testuser2", "password")};
    const auto [eventCreated, eventData]{createEvent("Test Event", "This is a test event", "Yes Share", "No Share")};

    Share share{eventData.yesShare.id};

    updateUserHoldings(userId1, eventData.yesShare.id, 1000);
    updateUserBalance(userId2, 49500);

    std::vector<std::shared_ptr<Order>> orders;

    for (int price{45}; price < 55; ++price) {
        auto order{std::make_shared<Order>(userId2, OrderType::BUY, OrderMode::LIMIT,
                         eventData.id, eventData.yesShare.id, 100, price)};
        share.addOrder(order);
        orders.push_back(order);
    }

    REQUIRE(share.getBuyOrderQuantity() == 1000);
    
    
    REQUIRE(share.getBuyPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {54, 100}, {53, 100}, {52, 100}, {51, 100}, {50, 100},
        {49, 100}, {48, 100}, {47, 100}, {46, 100}, {45, 100}});

    {
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::MARKET,
                         eventData.id, eventData.yesShare.id, 1000, 50)};

        auto filled{share.fillOrder(order)};

        REQUIRE(order->leftoverQuantitiy() == 500);

        REQUIRE(filled == 500);
        REQUIRE(share.getBuyOrderQuantity() == 500);
        REQUIRE(share.getBuyPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {49, 100}, {48, 100}, {47, 100}, {46, 100}, {45, 100}});

        REQUIRE(getUserBalance(userId2) == 49500 - 50 * 100 - 51 * 100 - 52* 100 - 53* 100 - 54*100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 500);

        REQUIRE(getUserBalance(userId1) == 50 * 100 + 51 * 100 + 52* 100 + 53* 100 + 54*100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 500);
    }

    {
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::MARKET,
                         eventData.id, eventData.yesShare.id, 1000, 50)};

        auto filled{share.fillOrder(order)};

        REQUIRE(order->leftoverQuantitiy() == 1000);

        REQUIRE(filled == 0);
        REQUIRE(share.getBuyOrderQuantity() == 500);
        REQUIRE(share.getBuyPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {49, 100}, {48, 100}, {47, 100}, {46, 100}, {45, 100}});

        REQUIRE(getUserBalance(userId2) == 49500 - 50 * 100 - 51 * 100 - 52* 100 - 53* 100 - 54*100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 500);

        REQUIRE(getUserBalance(userId1) == 50 * 100 + 51 * 100 + 52* 100 + 53* 100 + 54*100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 500);
    }

    {
        share.cancelOrder(orders[2]);
        orders.erase(orders.begin() + 2);

        REQUIRE(share.getBuyOrderQuantity() == 400);
        REQUIRE(share.getBuyPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {49, 100}, {48, 100}, {46, 100}, {45, 100}});
    }

    {
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::MARKET,
                         eventData.id, eventData.yesShare.id, 600, 45)};

        auto filled{share.fillOrder(order)};

        REQUIRE(order->leftoverQuantitiy() == 200);

        REQUIRE(filled == 400);
        REQUIRE(share.getBuyOrderQuantity() == 0);
        REQUIRE(share.getBuyPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{});

        REQUIRE(getUserBalance(userId2) == 4700);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 900);

        REQUIRE(getUserBalance(userId1) == 49500 - 4700);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 100);
    }
}

TEST_CASE("Share Sell fills Buy") {
    deleteDatabase();
    initializeDatabase();

    const auto [success1, userId1]{registerUser("testuser", "password")};
    const auto [success2, userId2]{registerUser("testuser2", "password")};
    const auto [eventCreated, eventData]{createEvent("Test Event", "This is a test event", "Yes Share", "No Share")};

    Share share{eventData.yesShare.id};

    updateUserHoldings(userId1, eventData.yesShare.id, 1000);
    updateUserBalance(userId2, 49500);

    std::vector<std::shared_ptr<Order>> orders;
    for (int price{45}; price < 55; ++price) {
        auto order{std::make_shared<Order>(userId1, OrderType::SELL, OrderMode::LIMIT,
                         eventData.id, eventData.yesShare.id, 100, price)};
        share.addOrder(order);
        orders.push_back(order);
    }

    REQUIRE(share.getSellOrderQuantity() == 1000);
    

    REQUIRE(share.getSellPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {45, 100}, {46, 100}, {47, 100}, {48, 100}, {49, 100},
        {50, 100}, {51, 100}, {52, 100}, {53, 100}, {54, 100}});

    {
        auto order{std::make_shared<Order>(userId2, OrderType::BUY, OrderMode::MARKET,
                         eventData.id, eventData.yesShare.id, 1000, 49)};

        auto filled{share.fillOrder(order)};

        REQUIRE(order->leftoverQuantitiy() == 500);

        REQUIRE(filled == 500);
        REQUIRE(share.getSellOrderQuantity() == 500);
        REQUIRE(share.getSellPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {50, 100}, {51, 100}, {52, 100}, {53, 100}, {54, 100}});

        REQUIRE(getUserBalance(userId2) == 49500 - 45 * 100 - 46 * 100 - 47* 100 - 48* 100 - 49*100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 500);

        REQUIRE(getUserBalance(userId1) == 45 * 100 + 46 * 100 + 47* 100 + 48* 100 + 49*100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 500);
    }

    {
        auto order{std::make_shared<Order>(userId2, OrderType::BUY, OrderMode::MARKET,
                         eventData.id, eventData.yesShare.id, 1000, 49)};

        auto filled{share.fillOrder(order)};

        REQUIRE(order->leftoverQuantitiy() == 1000);

        REQUIRE(filled == 0);
        REQUIRE(share.getSellOrderQuantity() == 500);
        REQUIRE(share.getSellPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {50, 100}, {51, 100}, {52, 100}, {53, 100}, {54, 100}});

        REQUIRE(getUserBalance(userId2) == 49500 - 45 * 100 - 46 * 100 - 47* 100 - 48* 100 - 49*100);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 500);

        REQUIRE(getUserBalance(userId1) == 45 * 100 + 46 * 100 + 47* 100 + 48* 100 + 49*100);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 500);
    }

    {
        share.cancelOrder(orders[8]);
        orders.erase(orders.begin() + 8);

        REQUIRE(share.getSellOrderQuantity() == 400);
        REQUIRE(share.getSellPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{
        {50, 100}, {51, 100}, {52, 100}, {54, 100}});
    }

    {
        auto order{std::make_shared<Order>(userId2, OrderType::BUY, OrderMode::MARKET,
                         eventData.id, eventData.yesShare.id, 600, 54)};

        auto filled{share.fillOrder(order)};

        REQUIRE(order->leftoverQuantitiy() == 200);

        REQUIRE(filled == 400);
        REQUIRE(share.getSellOrderQuantity() == 0);
        REQUIRE(share.getSellPricesAndQuantities() == std::vector<std::pair<uint16_t, uint64_t>>{});

        REQUIRE(getUserBalance(userId2) == 5300);
        REQUIRE(getUserShareHoldings(userId2, eventData.yesShare.id) == 900);

        REQUIRE(getUserBalance(userId1) == 49500 - 5300);
        REQUIRE(getUserShareHoldings(userId1, eventData.yesShare.id) == 100);
    }
}