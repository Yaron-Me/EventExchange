#include "Stock.hpp"

namespace exchange {
    Stock::Stock(const boost::uuids::uuid& _id,
                 const boost::uuids::uuid& _yesId,
                 const boost::uuids::uuid& _noId) :
        id(_id), yes(_yesId), no(_noId) {};
}