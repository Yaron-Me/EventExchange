#include <boost/uuid.hpp>

#include "Share.hpp"

namespace exchange {
    Share::Share(const boost::uuids::uuid& _id) : id{_id} {
        // Constructor implementation
    }
}