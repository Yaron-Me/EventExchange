#pragma once

#include <map>
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
            std::int64_t getTiedUpBalance();
            std::map<boost::uuids::uuid, std::uint32_t> getSellOrderShareCounts();
            void addOrder(std::weak_ptr<Order> order);
            std::size_t getOrderCount() const;

        private:
            std::vector<std::weak_ptr<Order>> orders;
    };
}
