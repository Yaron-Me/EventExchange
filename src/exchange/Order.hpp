#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <ostream>
#include <print>
#include <iostream>

#include "../utility/uuid.hpp"
#include "../exchange/User.hpp"

// Forward declarations to avoid circular dependencies
namespace exchange {
    class OrderQueue;
    class User;
}

namespace exchange {
    static const std::uint16_t MAX_DENOMINATIONS = 100;

    enum class OrderType {
        BUY,
        SELL
    };

    enum class OrderMode {
        MARKET,
        LIMIT
    };

    class Order {
        public:
            const boost::uuids::uuid id;
            const boost::uuids::uuid userId;
            const OrderType type;
            const OrderMode mode;
            const boost::uuids::uuid eventId;
            const boost::uuids::uuid shareId;
            const std::uint64_t quantity;
            const std::uint16_t price;

            Order(const boost::uuids::uuid& _userId, const OrderType _type, const OrderMode _mode,
                  const boost::uuids::uuid& _eventId, const boost::uuids::uuid& _shareId,
                  const std::uint64_t _quantity, const std::uint16_t _price);

            std::uint64_t leftoverQuantitiy() const;

            std::uint64_t positionValue() const;

            std::uint64_t totalTransactedValue() const;

            std::uint64_t fill(std::uint64_t quantityToFill, std::uint16_t price);

            ~Order();

        private:
            std::uint64_t filledQuantity;
            std::uint64_t transactedValue;
    };
}

template<>
struct std::formatter<exchange::OrderType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    
    template<typename FormatContext>
    auto format(const exchange::OrderType& type, FormatContext& ctx) {
        switch (type) {
            case exchange::OrderType::BUY:
                return std::format_to(ctx.out(), "BUY");
            case exchange::OrderType::SELL:
                return std::format_to(ctx.out(), "SELL");
        }
    }
};

template<>
struct std::formatter<exchange::OrderMode> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const exchange::OrderMode& mode, FormatContext& ctx) {
        switch (mode) {
            case exchange::OrderMode::MARKET:
                return std::format_to(ctx.out(), "MARKET");
            case exchange::OrderMode::LIMIT:
                return std::format_to(ctx.out(), "LIMIT");
        }
    }
};