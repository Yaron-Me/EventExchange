#pragma once

#include <boost/uuid.hpp>
#include <vector>

#include "Share.hpp"
#include "Order.hpp"

namespace exchange {
    class Stock {
        public:
            const boost::uuids::uuid id;

            Stock(const boost::uuids::uuid& _id,
                  const boost::uuids::uuid& _yesId,
                  const boost::uuids::uuid& _noId);

            const Share& getYesShare() const {
                return yes;
            }
            const Share& getNoShare() const {
                return no;
            }

        private:
            Share yes;
            Share no;
    };
}