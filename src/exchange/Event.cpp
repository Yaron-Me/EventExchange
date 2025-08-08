#include <iostream>

#include "Event.hpp"

namespace exchange {
    Event::Event(const boost::uuids::uuid& _id,
                 const boost::uuids::uuid& _yesId,
                 const boost::uuids::uuid& _noId) :
        id{_id}, yes{_yesId}, no{_noId} {}


    void Event::addOrder(std::shared_ptr<Order> order) {
        if (order->type == OrderType::BUY) {
            addBuyOrder(order);
        }
        else {
            addSellOrder(order);
        }
    }

    void Event::addBuyOrder(std::shared_ptr<Order> order) {
        std::cout << "Adding buy order to event: " << order->id << std::endl;
    }

    void Event::addSellOrder(std::shared_ptr<Order> order) {
        std::cout << "Adding sell order to event: " << order->id << std::endl;
    }
}