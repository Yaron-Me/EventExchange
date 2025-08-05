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
            boost::uuids::uuid id;

            std::int64_t getTiedUpBalance() const;

            std::map<boost::uuids::uuid, std::uint32_t> getSellOrderShareCounts() const;

        private:
            std::vector<std::shared_ptr<Order>> orders;
    };
}
