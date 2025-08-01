#include <boost/uuid.hpp>

namespace utility {
    const boost::uuids::uuid generateUUID() {
        static boost::uuids::random_generator generator;
        return generator();
    }

    const std::string uuidToString(const boost::uuids::uuid& uuid) {
        return boost::uuids::to_string(uuid);
    }
}