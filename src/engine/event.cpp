#include <print>

#include "event.hpp"

namespace engine {
    Event::Event(const boost::uuids::uuid& _id,
                 const boost::uuids::uuid& _yesId,
                 const boost::uuids::uuid& _noId) :
        id{_id}, yes{_yesId}, no{_noId} {}


    void Event::addOrder(std::shared_ptr<Order> order) {
        std::print("Adding order to event {}\n", utility::uuidToString(order->eventId));
    }
}