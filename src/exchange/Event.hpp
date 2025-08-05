#pragma once

#include <boost/uuid.hpp>
#include <vector>

#include "Share.hpp"
#include "Order.hpp"

namespace exchange {
    class Event {
        public:
            const boost::uuids::uuid id;

            Event(const boost::uuids::uuid& _id,
                  const boost::uuids::uuid& _yesId,
                  const boost::uuids::uuid& _noId);

            const Share& getYesShare() const {
                return yes;
            }
            const Share& getNoShare() const {
                return no;
            }

            Share& getShareById(const boost::uuids::uuid& shareId) {
                if (shareId == yes.id) {
                    return yes;
                }
                else if (shareId == no.id) {
                    return no;
                }
                else {
                    throw std::invalid_argument("Invalid share ID");
                }
            }

        private:
            Share yes;
            Share no;
    };
}