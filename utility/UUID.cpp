#include <boost/uuid.hpp>
#include <array>

namespace utility {
    const boost::uuids::uuid generateUUID() {
        static boost::uuids::random_generator generator;
        return generator();
    }
}