#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <boost/uuid.hpp>

#include "exchange/Order.hpp"

namespace exchange {
    class User {
        public:
            boost::uuids::uuid id;

        private:
            std::vector<std::shared_ptr<exchange::Order>> orders;
    };
}
