#include <string>
#include <cassert>
#include <deque>
#include <unordered_map>
#include <map>
#include <vector>

#include <memory>
#include <iostream>
#include <unordered_set>
#include <optional>
#include <ranges>

#include <boost/uuid.hpp>

static const int maxDenominations = 100;

enum class OrderType {
    BUY,
    SELL
};

void grantShares(const std::string& userId, const std::string& stockName,
                const std::string& shareName, int amount) {
    // Placeholder for granting shares to the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)stockName;
    (void)shareName;
    (void)amount;
}

void grantFunds(const std::string& userId, int amount) {
    // Placeholder for granting funds to the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)amount;
}

void removeShares(const std::string& userId, const std::string& stockName,
                const std::string& shareName, int amount) {
    // Placeholder for removing shares from the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)stockName;
    (void)shareName;
    (void)amount;
}

void removeFunds(const std::string& userId, int amount) {
    // Placeholder for removing funds from the user
    // In a real implementation, this would update the user's account in db
    (void)userId;
    (void)amount;
}

std::string generateUUID() {
    static boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    std::string uuidStr = boost::uuids::to_string(uuid);
    return uuidStr;
}

class Order {
    public:
        const std::string orderId;
        const std::string userId;
        const OrderType type;
        const std::string stockName;
        const std::string shareName;
        const int orderedAmount;
        const int priceOrderedAt;

        Order(const std::string& _userId, OrderType _type, const std::string& _stockName,
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

        ~Order() {
            // std::cout << "Order " << orderId << " is being destroyed" << std::endl;
        }

        // Fill the order, returns the amount filled, updates Database
        int fill(int amountToFill, int price) {
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
                grantShares(userId, stockName, shareName, filledAmount);
                removeFunds(userId, filledAmount * price);
            }
            else if (type == OrderType::SELL) {
                removeShares(userId, stockName, shareName, filledAmount);
                grantFunds(userId, filledAmount * price);
            }
            totalMoneyFlow += filledAmount * price;

            return filledAmount;
        }

        bool isFilled() const {
            assert(leftToOrder >= 0 && "leftToOrder cannot be negative");
            return leftToOrder == 0;
        }

        int getLeftToOrder() const {
            return leftToOrder;
        }

        float getAveragePrice() const {
            if (orderedAmount - leftToOrder == 0) {
                return 0;
            }
            return static_cast<float>(totalMoneyFlow) /
                   static_cast<float>(orderedAmount - leftToOrder);
        }

    private:
        int leftToOrder;
        int totalMoneyFlow;
};

class OrderQueue {
    public:
        void addOrder(const std::shared_ptr<Order>& order) {
            orderQueue.push_back(order);
        }

        // Fill the orders in the queue with the given amount
        // returns the amount filled
        int fillOrders(int amountToFill) {
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

        bool removeOrder(const std::shared_ptr<Order>& order) {
            if (std::erase(orderQueue, order) == 0) {
                return false;  // Order not found.
            }
            return true;  // Order removed.
        }

        bool isEmpty() const {
            return orderQueue.empty();
        }

        int getShareCount() const {
            int totalShares {0};

            for (const auto& order : orderQueue) {
                totalShares += order->getLeftToOrder();
            }

            return totalShares;
        }

    private:
        std::deque<std::shared_ptr<Order>> orderQueue;
};

class Share {
    public:
        const std::string name;

        Share(const std::string& _name) : name{_name} {
            assert(!_name.empty() && "Share name cannot be empty");
        }

        bool addOrder(const std::shared_ptr<Order>& order) {
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

        bool removeOrder(const std::shared_ptr<Order>& order) {
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

        bool enoughAvailableSharesToBuy(int amount, int price) const {
            int availableShares {0};

            for (auto& [queuePrice, orderQueue] : sellOrders){
                if (availableShares >= amount || queuePrice > price) {
                    break;
                }
                availableShares += orderQueue.getShareCount();
            }

            return availableShares >= amount;
        }

        bool enoughAvailableSharesToSell(int amount, int price) const {
            int availableShares {0};

            for (auto& [queuePrice, orderQueue] : std::views::reverse(buyOrders)){
                if (availableShares >= amount || queuePrice < price) {
                    break;
                }
                availableShares += orderQueue.getShareCount();
            }

            return availableShares >= amount;
        }

        int getBestSellPrice() const {
            return buyOrders.empty() ? 0 : buyOrders.rbegin()->first;
        }
        int getBestBuyPrice() const {
            return sellOrders.empty() ? maxDenominations + 1 : sellOrders.begin()->first;
        }

    private:
        std::map<int, OrderQueue> buyOrders;
        std::map<int, OrderQueue> sellOrders;

        int tryFillOrder(const std::shared_ptr<Order>& order) {
            int totalFilled{0};
            std::vector<int> queuesToRemove;

            if (order->type == OrderType::BUY) {
                for ( auto& [price, queue] : sellOrders) {
                    if (price > order->priceOrderedAt || order->isFilled()) break;

                    // fill Orders in orderbook
                    int filledAmount = queue.fillOrders(order->getLeftToOrder());

                    // fill the order were trying to fill
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
};

class Stock {
    public:
        const std::string name;

        Stock(const std::string& _name, const std::vector<std::string>& shareNames) :
            name{_name} {
            assert(!_name.empty() && "Stock name cannot be empty");
            for (const auto& shareName : shareNames) {
                shares.emplace_back(shareName);
            }
        }

        bool addOrder(const std::shared_ptr<Order>& order) {
            for (auto& share : shares) {
                if (share.name == order->shareName) {
                    return share.addOrder(order);
                }
            }
            return false; // Share not found
        }

        bool removeOrder(const std::shared_ptr<Order>& order) {
            for (auto& share : shares) {
                if (share.name == order->shareName) {
                    return share.removeOrder(order);
                }
            }
            return false;
        }

    private:
        std::vector<Share> shares;
};

class UserOrders {
    public:
        void addOrder(const std::shared_ptr<Order>& order) {
            OrdersPerUser[order->userId].push_back(order);
        }

        bool removeOrder(const std::shared_ptr<Order>& order) {
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

        std::shared_ptr<Order> getOrder(const std::string& userId, const std::string& orderId) {
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

        std::vector<std::shared_ptr<Order>>& getUserOrders(const std::string& userId) {
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

        std::vector<std::string> getUserIds() {
            std::vector<std::string> userIds;
            for (const auto& pair : OrdersPerUser) {
                userIds.push_back(pair.first);
            }
            return userIds;
        }

        std::size_t totalOrders() {
            std::size_t total {0};
            for (const auto& userId : getUserIds()) {
                total += getUserOrders(userId).size();
            }
            return total;
        }

    private:
        std::unordered_map<std::string, std::vector<std::shared_ptr<Order>>> OrdersPerUser;
};

class Exchange {
    public:
        bool createStock(const std::string& stockName, const std::vector<std::string>& shareNames) {auto stockOpt = getStock(stockName);
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

        bool createOrder(const std::string& userId, OrderType type,
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

        bool cancelOrder(const std::string& userId, const std::string& orderId) {
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

        std::optional<std::reference_wrapper<Stock>> getStock(const std::string& stockName) {
            auto it {std::ranges::find_if(stocks, [&stockName](const Stock& stock) {
                return stock.name == stockName;
            })};

            if (it != stocks.end()) {
                return std::ref(*it);
            }
            return std::nullopt;
        }

        std::size_t totalOrders() {
            return userOrders.totalOrders();
        }

    private:
        std::vector<Stock> stocks;
        UserOrders userOrders;
};

int main() {
    // Create an order
    Exchange exchange;

    int STOCKS = 10;
    int SHARES = 3;
    int ORDERS = 10;
    int USERS = 10000;
    int AMOUNT = 10;

    int totalOrders = STOCKS * SHARES * ORDERS * USERS;
    std::cout << "Total orders: " << totalOrders << std::endl;

    for (int i = 0; i < STOCKS; ++i) {
        std::string stockName = "Stock" + std::to_string(i);
        std::vector<std::string> shareNames;
        for (int j = 0; j < SHARES; ++j) {
            shareNames.push_back("Share" + std::to_string(j));
        }
        exchange.createStock(stockName, shareNames);
    }
    for (int l = 0; l < ORDERS; ++l) {
        for (int i = 0; i < USERS; ++i) {
            std::string userId = "User" + std::to_string(i);
            for (int j = 0; j < STOCKS; ++j) {
                std::string stockName = "Stock" + std::to_string(j);
                for (int k = 0; k < SHARES; ++k) {
                    std::string shareName = "Share" + std::to_string(k);
                    int amount = AMOUNT;
                    int price = l + 1;
                    exchange.createOrder(userId, OrderType::BUY, stockName, shareName, amount, price);
                }
            }
        }
    }

    std::cout << "Total orders after creation: " << exchange.totalOrders() << std::endl;

    // Fill orders
    for (int l = 0; l < ORDERS; ++l) {
        for (int i = 0; i < USERS; ++i) {
            std::string userId = "User" + std::to_string(i);
            for (int j = 0; j < STOCKS; ++j) {
                std::string stockName = "Stock" + std::to_string(j);
                for (int k = 0; k < SHARES; ++k) {
                    std::string shareName = "Share" + std::to_string(k);
                    int amount = AMOUNT;
                    int price = ORDERS - l;
                    exchange.createOrder(userId, OrderType::SELL, stockName, shareName, amount, price);
                }
            }
        }
    }

    std::cout << "Total orders after filling: " << exchange.totalOrders() << std::endl;

    return 0;
}