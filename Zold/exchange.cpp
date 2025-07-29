#include <string>
#include <cassert>
#include <deque>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <unordered_set>
#include <optional>
#include <ranges>

#include <boost/uuid.hpp>
#include "database.hpp"

#include <iostream>

namespace exchange {

std::string generateUUID() {
    static boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    std::string uuidStr = boost::uuids::to_string(uuid);
    return uuidStr;
}

Order::Order(const std::string& _userId, OrderType _type, const std::string& _stockName,
              const std::string& _shareName, int amount, int price) :
              orderId{generateUUID()}, userId{_userId}, type{_type}, stockName{_stockName},
              shareName{_shareName}, orderedAmount{amount},
              priceOrderedAt{price}, leftToOrder{amount}, totalMoneyFlow{0} {
            assert(!orderId.empty() && "orderId cannot be empty");
            assert(!_userId.empty() && "userId cannot be empty");
            assert(!_stockName.empty() && "stockName cannot be empty");
            assert(!_shareName.empty() && "shareName cannot be empty");
            assert(amount > 0 && "amount must be positive");
            assert(price > 0 && price <= maxDenominations && "price out of valid range");
        }

Order::~Order() {
    database::uploadFinishedOrder(this);
}

// Fill the order, returns the amount filled, updates Database
int Order::fill(int amountToFill, int price) {
    if (type == OrderType::BUY) {
        assert(price <= priceOrderedAt && "Price must be less than or equal to the ordered price");
    }
    else if (type == OrderType::SELL) {
        assert(price >= priceOrderedAt && "Price must be greater than or equal to the ordered price");
    }

    if (isFilled()) {
        return 0;
    }

    int filledAmount{0};

    if (amountToFill >= leftToOrder) {
        filledAmount = leftToOrder;
        leftToOrder = 0;
    }
    else {
        leftToOrder -= amountToFill;
        filledAmount = amountToFill;
    }

    if (type == OrderType::BUY) {
        database::grantShares(userId, stockName, shareName, filledAmount);
        database::removeFunds(userId, filledAmount * price);
    }
    else if (type == OrderType::SELL) {
        database::removeShares(userId, stockName, shareName, filledAmount);
        database::grantFunds(userId, filledAmount * price);
    }
    totalMoneyFlow += filledAmount * price;

    return filledAmount;
}

bool Order::isFilled() const {
    assert(leftToOrder >= 0 && "leftToOrder cannot be negative");
    return leftToOrder == 0;
}

int Order::getLeftToOrder() const {
    return leftToOrder;
}

float Order::getAveragePrice() const {
    if (orderedAmount - leftToOrder == 0) {
        return 0;
    }
    return static_cast<float>(totalMoneyFlow) /
            static_cast<float>(orderedAmount - leftToOrder);
}

void OrderQueue::addOrder(const std::shared_ptr<Order>& order) {
    orderQueue.push_back(order);
}

// Fill the orders in the queue with the given amount
// returns the amount filled
int OrderQueue::fillOrders(int amountToFill) {
    int filledAmount {0};

    while (!orderQueue.empty() && amountToFill - filledAmount > 0) {
        auto order {orderQueue.front()};
        int filled {order->fill(amountToFill, order->priceOrderedAt)};
        filledAmount += filled;

        if (order->isFilled()) {
            orderQueue.pop_front();
        }
    }

    return filledAmount;
}

bool OrderQueue::removeOrder(const std::shared_ptr<Order>& order) {
    if (std::erase(orderQueue, order) == 0) {
        return false;  // Order not found.
    }
    return true;  // Order removed.
}

bool OrderQueue::isEmpty() const {
    return orderQueue.empty();
}

int OrderQueue::getShareCount() const {
    int totalShares {0};

    for (const auto& order : orderQueue) {
        totalShares += order->getLeftToOrder();
    }

    return totalShares;
}

Share::Share(const std::string& _name) : name{_name} {
    assert(!_name.empty() && "Share name cannot be empty");
}

bool Share::addOrder(const std::shared_ptr<Order>& order) {
    tryFillOrder(order);

    if (order->isFilled()) {
        return false; // Order was filled and should not be added to the queue, this also signals that it shouldnt be added to the active orders
    }

    if (order->type == OrderType::BUY) {
        buyOrders[order->priceOrderedAt].addOrder(order);
    }
    else if (order->type == OrderType::SELL) {
        sellOrders[order->priceOrderedAt].addOrder(order);
    }
    return true;
}

int Share::tryFillOrder(const std::shared_ptr<Order>& order) {
    int totalFilled{0};
    std::vector<int> queuesToRemove;

    if (order->type == OrderType::BUY) {
        for (auto& [price, queue] : sellOrders) {
            if (price > order->priceOrderedAt || order->isFilled()) break;

            int filledAmount = queue.fillOrders(order->getLeftToOrder());
            totalFilled += order->fill(filledAmount, price);

            if (queue.isEmpty()) {
                queuesToRemove.push_back(price);
            }
        }
        for (int price : queuesToRemove) {
            sellOrders.erase(price);
        }
    }
    else if (order->type == OrderType::SELL) {
        for (auto& [price, queue] : std::views::reverse(buyOrders)) {
            if (price < order->priceOrderedAt || order->isFilled()) break;

            int filledAmount = queue.fillOrders(order->getLeftToOrder());
            totalFilled += order->fill(filledAmount, price);

            if (queue.isEmpty()) {
                queuesToRemove.push_back(price);
            }
        }
        for (int price : queuesToRemove) {
            buyOrders.erase(price);
        }
    }

    return totalFilled;
}

bool Share::removeOrder(const std::shared_ptr<Order>& order) {
    if (order->type == OrderType::BUY) {
        if(!buyOrders[order->priceOrderedAt].removeOrder(order)) {
            return false; // Order not found in the queue
        }

        if (buyOrders[order->priceOrderedAt].isEmpty()) {
            buyOrders.erase(order->priceOrderedAt);
        }
    }
    else if (order->type == OrderType::SELL) {
        if(!sellOrders[order->priceOrderedAt].removeOrder(order)) {
            return false; // Order not found in the queue
        }

        if (sellOrders[order->priceOrderedAt].isEmpty()) {
            sellOrders.erase(order->priceOrderedAt);
        }
    }
    return true;
}

bool Share::enoughAvailableSharesToBuy(int amount, int price) const {
    int availableShares {0};

    for (auto& [queuePrice, orderQueue] : sellOrders){
        if (availableShares >= amount || queuePrice > price) {
            break;
        }
        availableShares += orderQueue.getShareCount();
    }

    return availableShares >= amount;
}

bool Share::enoughAvailableSharesToSell(int amount, int price) const {
    int availableShares {0};

    for (auto& [queuePrice, orderQueue] : std::views::reverse(buyOrders)){
        if (availableShares >= amount || queuePrice < price) {
            break;
        }
        availableShares += orderQueue.getShareCount();
    }

    return availableShares >= amount;
}

bool Share::fillToMatch(int amount, int price) {
    if (!buyOrders.contains(price)) {
        return false;
    }

    auto& queue {buyOrders.at(price)};
    if (queue.getShareCount() < amount) {
        return false;
    }

    return queue.fillOrders(amount) == amount;
}

std::vector<std::pair<int, int>> Share::getBuySummary() const {
    std::vector<std::pair<int, int>> priceAmount;
    for (const auto& [price, queue] : buyOrders) {
        priceAmount.emplace_back(price, queue.getShareCount());
    }
    return priceAmount;
}
std::vector<std::pair<int, int>> Share::getSellSummary() const {
    std::vector<std::pair<int, int>> priceAmount;
    for (const auto& [price, queue] : sellOrders) {
        priceAmount.emplace_back(price, queue.getShareCount());
    }
    return priceAmount;
}

std::pair<int, int> Share::maxBuyPriceAndAmount() const {
    auto highestEntry = buyOrders.rbegin();

    if (highestEntry == buyOrders.rend()) {
        return {0, 0};
    }

    return {highestEntry->first, highestEntry->second.getShareCount()};
}

Stock::Stock(const std::string& _name, const std::vector<std::string>& shareNames) :
    name{_name} {
    assert(!_name.empty() && "Stock name cannot be empty");
    for (const auto& shareName : shareNames) {
        shares.emplace_back(shareName);
    }
}

bool Stock::addOrder(const std::shared_ptr<Order>& order) {
    auto shareOpt {getShare(order->shareName)};

    if (!shareOpt) {
        return false; // Share not found
    }

    if (order->type == OrderType::BUY) {
        tryMatchOrder(order);
        if (order->isFilled()) {
            return false; // order already filled
        }
    }

    return shareOpt.value().get().addOrder(order);
}

void Stock::tryMatchOrder(const std::shared_ptr<Order>& order) {
    std::vector<std::pair<int, int>> maxPricesAndAmounts;

    auto priceSum = [](std::vector<std::pair<int, int>> pairList) {
        int sum {0};
        for (auto& [price, amount] : pairList) {
            sum += price;
        }
        return sum;
    };

    auto lowestAmount = [](std::vector<std::pair<int, int>> pairList) {
        int lowest {pairList[0].second};
        for (auto& [price, amount] : pairList) {
            if (amount != 0 && amount < lowest) {
                lowest = amount;
            }
        }
        return lowest;
    };

    for (auto share : shares) {
        if (share.name != order->shareName) {
            maxPricesAndAmounts.emplace_back(share.maxBuyPriceAndAmount());
        }
    }

    int sum {priceSum(maxPricesAndAmounts)};

    while (sum + order->priceOrderedAt >= maxDenominations) {
        if (order->isFilled()) {
            break;
        }
        int minAmount = lowestAmount(maxPricesAndAmounts);
        if (minAmount == 0) {
            break;
        }

        order->fill(minAmount, maxDenominations - sum);

        size_t i {0};
        for (auto& share : shares) {
            if (share.name != order->shareName) {
                share.fillToMatch(minAmount, maxPricesAndAmounts[i].first);
                ++i;
            }
        }

        maxPricesAndAmounts.clear();
        for (auto& share : shares) {
            maxPricesAndAmounts.emplace_back(share.maxBuyPriceAndAmount());
        }
        sum = priceSum(maxPricesAndAmounts);
    }
}

bool Stock::removeOrder(const std::shared_ptr<Order>& order) {
    auto shareOpt {getShare(order->shareName)};

    if (!shareOpt) {
        return false; // Share not found
    }

    return shareOpt.value().get().removeOrder(order);
}

std::optional<std::reference_wrapper<Share>> Stock::getShare(const std::string& shareName) {
    auto it {std::ranges::find_if(shares, [&shareName](const Share& share) {
        return share.name == shareName;
    })};

    if (it != shares.end()) {
        return std::ref(*it);
    }
    return std::nullopt;
}



void UserOrders::addOrder(const std::shared_ptr<Order>& order) {
    OrdersPerUser[order->userId].push_back(order);
}

bool UserOrders::removeOrder(const std::shared_ptr<Order>& order) {
    auto& userOrders {getUserOrders(order->userId)};
    if (userOrders.empty()) {
        return false;
    }

    auto it {std::find(userOrders.begin(), userOrders.end(), order)};
    if (it == userOrders.end()) {
        return false;
    }

    userOrders.erase(it);
    if (userOrders.empty()) { // remove the user from the map
        OrdersPerUser.erase(order->userId);
    }
    return true;
}

std::shared_ptr<Order> UserOrders::getOrder(const std::string& userId, const std::string& orderId) {
    auto& userOrders {getUserOrders(userId)};
    if (userOrders.empty()) {
        return nullptr;
    }

    for (auto& order : userOrders) {
        if (order->orderId == orderId) {
            return order;
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<Order>>& UserOrders::getUserOrders(const std::string& userId) {
    auto& userOrders {OrdersPerUser[userId]};

    // Remove all Orders that are filled
    std::erase_if(userOrders, [](auto& order) { return order->isFilled(); });

    if (userOrders.empty()) {
        OrdersPerUser.erase(userId);
        static std::vector<std::shared_ptr<Order>> emptyOrders;
        return emptyOrders;
    }

    return userOrders;
}

std::vector<std::string> UserOrders::getUserIds() {
    std::vector<std::string> userIds;
    for (const auto& pair : OrdersPerUser) {
        userIds.push_back(pair.first);
    }
    return userIds;
}

std::size_t UserOrders::totalOrders() {
    std::size_t total {0};
    for (const auto& userId : getUserIds()) {
        total += getUserOrders(userId).size();
    }
    return total;
}

bool Exchange::createStock(const std::string& stockName, const std::vector<std::string>& shareNames) {
    auto stockOpt = getStock(stockName);

    if (stockOpt) {
        return false; // Stock already exists
    }
    if (shareNames.empty()) {
        return false; // No shares provided
    }
    for (const auto& shareName : shareNames) {
        if (shareName.empty()) {
            return false; // Share name cannot be empty
        }
    }

    std::unordered_set<std::string> seenNames;
    for (const auto& name : shareNames) {
        if (!seenNames.insert(name).second) {
            return false; // Duplicate found
        }
    }

    stocks.emplace_back(stockName, shareNames);
    return true; // Stock created successfully
}

bool Exchange::createOrder(const std::string& userId, OrderType type,
                const std::string& stockName, const std::string& shareName,
                int amount, int price) {
    auto stockOpt {getStock(stockName)};
    if (!stockOpt) {
        return false; // Stock not found
    }

    auto order {std::make_shared<Order>(userId, type, stockName, shareName, amount, price)};

    if (!stockOpt.value().get().addOrder(order)) {
        return false;
    }
    userOrders.addOrder(order);

    return true; // Order could not be added
}

bool Exchange::cancelOrder(const std::string& userId, const std::string& orderId) {
    auto order {userOrders.getOrder(userId, orderId)};
    if (!order) {
        return false; // Order not found
    }
    auto stockOpt {getStock(order->stockName)};
    if (!stockOpt) {
        return false; // Stock not found
    }

    return userOrders.removeOrder(order) && stockOpt.value().get().removeOrder(order);
}

std::optional<std::reference_wrapper<Stock>> Exchange::getStock(const std::string& stockName) {
    auto it {std::ranges::find_if(stocks, [&stockName](const Stock& stock) {
        return stock.name == stockName;
    })};

    if (it != stocks.end()) {
        return std::ref(*it);
    }
    return std::nullopt;
}

std::size_t Exchange::totalOrders() {
    return userOrders.totalOrders();
}

} // namespace exchange
