#include <boost/uuid.hpp>

namespace utility {
    const boost::uuids::uuid generateUUID() {
        static boost::uuids::random_generator generator;
        return generator();
    }
}