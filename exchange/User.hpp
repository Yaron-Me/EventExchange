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

        private:
            std::vector<std::shared_ptr<Order>> orders;
    };
}
