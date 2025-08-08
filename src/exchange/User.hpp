#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <boost/uuid.hpp>

// Forward declaration to avoid circular dependencies
namespace exchange {
    class Order;
}

namespace exchange {
    class User {
        public:
            std::int64_t getTiedUpBalance() const;
            std::map<boost::uuids::uuid, std::uint32_t> getSellOrderShareCounts() const;
            std::size_t getOrderCount() const;
            void addOrder(std::shared_ptr<Order> order);

        private:
            std::vector<std::shared_ptr<Order>> orders;
    };
}
