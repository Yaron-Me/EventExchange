#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <deque>
#include <unordered_map>
#include <map>


namespace engine {

static const int maxDenominations = 100;

enum class OrderType {
    BUY,
    SELL
};

std::string generateUUID();

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
                const std::string& _shareName, int amount, int price);

        ~Order();

        // Fill the order, returns the amount filled, updates Database
        int fill(int amountToFill, int price);

        bool isFilled() const;

        int getLeftToOrder() const;

        float getAveragePrice() const;

    private:
        int leftToOrder;
        int totalMoneyFlow;
};

class OrderQueue {
    public:
        void addOrder(const std::shared_ptr<Order>& order);

        // Fill the orders in the queue with the given amount
        // returns the amount filled
        int fillOrders(int amountToFill);

        bool removeOrder(const std::shared_ptr<Order>& order);
        bool isEmpty() const;

        int getShareCount() const;

    private:
        std::deque<std::shared_ptr<Order>> orderQueue;
};

class Share {
    public:
        const std::string name;

        Share(const std::string& _name);

        bool addOrder(const std::shared_ptr<Order>& order);

        bool removeOrder(const std::shared_ptr<Order>& order);

        bool enoughAvailableSharesToBuy(int amount, int price) const;

        bool enoughAvailableSharesToSell(int amount, int price) const;

        bool fillToMatch(int amount, int price);

        std::vector<std::pair<int, int>> getSellSummary() const;
        std::vector<std::pair<int, int>> getBuySummary() const;
        std::pair<int, int> maxBuyPriceAndAmount() const;

    private:
        std::map<int, OrderQueue> buyOrders;
        std::map<int, OrderQueue> sellOrders;

        int tryFillOrder(const std::shared_ptr<Order>& order);
};

class Stock {
    public:
        const std::string name;

        Stock(const std::string& _name, const std::vector<std::string>& shareNames);

        bool addOrder(const std::shared_ptr<Order>& order);

        bool removeOrder(const std::shared_ptr<Order>& order);

        std::optional<std::reference_wrapper<Share>> getShare(const std::string& shareName);

    private:
        std::vector<Share> shares;
        void tryMatchOrder(const std::shared_ptr<Order>& order);


};

class UserOrders {
    public:
        void addOrder(const std::shared_ptr<Order>& order);
        bool removeOrder(const std::shared_ptr<Order>& order);

        std::shared_ptr<Order> getOrder(const std::string& userId, const std::string& orderId);

        std::vector<std::shared_ptr<Order>>& getUserOrders(const std::string& userId);
        std::vector<std::string> getUserIds();

        std::size_t totalOrders();

    private:
        std::unordered_map<std::string, std::vector<std::shared_ptr<Order>>> OrdersPerUser;
};

class Exchange {
    public:
        bool createStock(const std::string& stockName, const std::vector<std::string>& shareNames);

        bool createOrder(const std::string& userId, OrderType type,
                        const std::string& stockName, const std::string& shareName,
                        int amount, int price);

        bool cancelOrder(const std::string& userId, const std::string& orderId);

        std::optional<std::reference_wrapper<Stock>> getStock(const std::string& stockName);

        std::size_t totalOrders();

    private:
        std::vector<Stock> stocks;
        UserOrders userOrders;
};

}