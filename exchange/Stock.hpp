#pragma once

#include <boost/uuid.hpp>
#include <vector>

#include "exchange/Share.hpp"
#include "exchange/Order.hpp"

namespace exchange {
    class Stock {
        public:
            const boost::uuids::uuid id;

            Stock(const boost::uuids::uuid& _id,
                  const boost::uuids::uuid& _yesId,
                  const boost::uuids::uuid& _noId);

        private:
            Share yes;
            Share no;
    };
}