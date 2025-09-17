#include <print>

#include "event.hpp"

namespace engine {
    Event::Event(const boost::uuids::uuid& _id,
                 const boost::uuids::uuid& _yesId,
                 const boost::uuids::uuid& _noId) :
        id{_id}, yes{_yesId}, no{_noId} {}


    void Event::addOrder(std::shared_ptr<Order>& order) {
        // Before we add the order we have to first match it with all other orders
        Share& thisShare = (order->shareId == yes.id) ? yes : no;
        Share& otherShare = (order->shareId == yes.id) ? no : yes;

        // if (order->type == OrderType::BUY) {
        //     auto sellPricesAndQuantities = thisShare.getSellPricesAndQuantities(order->leftoverQuantitiy(), order->price);
        //     auto buyPricesAndQuantities = otherShare.getBuyPricesAndQuantities(order->leftoverQuantitiy(), MAX_DENOMINATIONS - order->price, true);

            
        // }
        // else if (order->type == OrderType::SELL) {
        //     auto buyPricesAndQuantities = thisShare.getBuyPricesAndQuantities(order->leftoverQuantitiy(), order->price);
        //     auto sellPricesAndQuantities = otherShare.getSellPricesAndQuantities(order->leftoverQuantitiy(), MAX_DENOMINATIONS - order->price, true);
        // }

        if (order->mode == OrderMode::LIMIT && order->leftoverQuantitiy() > 0) {
            thisShare.addOrder(order);
        }
    }
}