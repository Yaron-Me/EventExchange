#include <catch2/catch_test_macros.hpp>
#include <random>
#include <chrono>
#include <vector>
#include <print>

#include "../src/engine/event.hpp"
#include "../src/engine/order.hpp"
#include "../src/utility/uuid.hpp"
#include "../src/database/utility.hpp"
#include "../src/database/user.hpp"
#include "../src/database/event.hpp"

using namespace utility;
using namespace engine;
using namespace database;

TEST_CASE("Benchmark: 100 users, 100,000 random orders") {
    // Setup database
    deleteDatabase();
    initializeDatabase();

    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> balanceDist(50000000, 200000000);  // 500k to 2M balance
    std::uniform_int_distribution<uint32_t> holdingsDist(1000, 10000);         // 1k to 10k shares
    std::uniform_int_distribution<uint64_t> quantityDist(1, 500);              // Order quantities 1-500
    std::uniform_int_distribution<uint16_t> priceDist(100, 900);               // Prices 100-900
    std::uniform_int_distribution<int> userDist(0, 99);                        // Pick random user
    std::uniform_int_distribution<int> typeDist(0, 1);                         // BUY or SELL
    std::uniform_int_distribution<int> modeDist(0, 1);                         // MARKET or LIMIT
    std::uniform_int_distribution<int> shareDist(0, 1);                        // YES or NO share

    std::print("=== EventExchange Benchmark Test ===\n");
    std::print("Creating test event and shares...\n");

    // Create test event
    const auto [eventCreated, eventData] = createEvent(
        "Benchmark Event", 
        "Large scale benchmark test event", 
        "Yes Share", 
        "No Share"
    );
    REQUIRE(eventCreated);

    std::print("Event created: {}\n", eventData.name);

    // Create 100 users with random balances and holdings
    std::vector<boost::uuids::uuid> userIds;
    userIds.reserve(100);

    std::print("Creating 100 users with funds and holdings...\n");
    
    for (int i = 0; i < 100; ++i) {
        std::string username = "user" + std::to_string(i);
        std::string password = "pass" + std::to_string(i);
        
        const auto [success, userId] = registerUser(username, password);
        REQUIRE(success);
        userIds.push_back(userId);

        // Give random balance
        uint64_t balance = balanceDist(gen);
        updateUserBalance(userId, balance);

        // Give random holdings of both shares
        uint32_t yesHoldings = holdingsDist(gen);
        uint32_t noHoldings = holdingsDist(gen);
        updateUserHoldings(userId, eventData.yesShare.id, yesHoldings);
        updateUserHoldings(userId, eventData.noShare.id, noHoldings);

        if (i % 20 == 19) {
            std::print("Created {} users...\n", i + 1);
        }
    }

    // Initialize event engine
    Event event{eventData.id, eventData.yesShare.id, eventData.noShare.id};

    std::print("Starting benchmark: 10,0000 random orders...\n");
    auto startTime = std::chrono::high_resolution_clock::now();

    // Generate 10,000 random orders
    uint64_t totalOrders = 100000;
    uint64_t completedOrders = 0;
    uint64_t matchedOrders = 0;

    for (uint64_t i = 0; i < totalOrders; ++i) {
        // Pick random parameters
        const auto& randomUserId = userIds[userDist(gen)];
        OrderType orderType = (typeDist(gen) == 0) ? OrderType::BUY : OrderType::SELL;
        OrderMode orderMode = (modeDist(gen) == 0) ? OrderMode::MARKET : OrderMode::LIMIT;
        const auto& shareId = (shareDist(gen) == 0) ? eventData.yesShare.id : eventData.noShare.id;
        uint64_t quantity = quantityDist(gen);
        uint16_t price = priceDist(gen);

        // Create and add order
        auto order = std::make_shared<Order>(
            randomUserId, orderType, orderMode, 
            eventData.id, shareId, quantity, price
        );

        uint64_t initialLeftover = order->leftoverQuantitiy();
        event.addOrder(order);
        uint64_t finalLeftover = order->leftoverQuantitiy();

        // Track statistics
        completedOrders++;
        if (finalLeftover < initialLeftover) {
            matchedOrders++;
        }

        // Progress reporting
        if (i % 10000 == 9999) {
            std::print("Processed {} orders ({:.1f}%)...\n", 
                i + 1, (double)(i + 1) / totalOrders * 100.0);
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Get final order book state
    auto [yesBuy, yesSell] = event.getBuyAndSellQuantities(eventData.yesShare.id);
    auto [noBuy, noSell] = event.getBuyAndSellQuantities(eventData.noShare.id);

    std::print("\n=== Benchmark Results ===\n");
    std::print("Total orders processed: {}\n", completedOrders);
    std::print("Orders with matches: {}\n", matchedOrders);
    std::print("Match rate: {:.2f}%\n", (double)matchedOrders / completedOrders * 100.0);
    std::print("Time taken: {} ms\n", duration.count());
    std::print("Orders per second: {:.0f}\n", (double)completedOrders / duration.count() * 1000.0);
    
    std::print("\nFinal Order Book State:\n");
    std::print("YES share - Buy orders: {}, Sell orders: {}\n", yesBuy, yesSell);
    std::print("NO share - Buy orders: {}, Sell orders: {}\n", noBuy, noSell);
    
    // Get final event statistics
    auto finalEventData = getEvent(eventData.id);
    std::print("Total shares issued: {}\n", finalEventData.issued);

    // Sample user balance verification
    std::print("\nSample user balances after trading:\n");
    for (int i = 0; i < 5; ++i) {
        auto balance = getUserBalance(userIds[i]);
        auto yesShares = getUserShareHoldings(userIds[i], eventData.yesShare.id);
        auto noShares = getUserShareHoldings(userIds[i], eventData.noShare.id);
        std::print("User {}: Balance={}, YES={}, NO={}\n", i, balance, yesShares, noShares);
    }

    // Performance requirements - these are just basic sanity checks
    REQUIRE(completedOrders == totalOrders);
    REQUIRE(duration.count() < 60000); // Should complete within 60 seconds
    REQUIRE(yesBuy + yesSell + noBuy + noSell > 0); // Should have some orders in book
    
    std::print("\n✅ Benchmark completed successfully!\n");
}
