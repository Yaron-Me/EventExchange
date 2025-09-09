#include <boost/uuid.hpp>
#include <print>
#include <iostream>

#include "share.hpp"

namespace exchange {
    Share::Share(const boost::uuids::uuid& _id) : id{_id} {}

    void Share::addOrder(std::shared_ptr<Order> order) {
        std::print("Adding order to share: {}\n", utility::uuidToString(order->id));
    }
}