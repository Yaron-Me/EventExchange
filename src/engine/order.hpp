#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <ostream>
#include <print>
#include <iostream>

#include "../utility/uuid.hpp"
#include "../engine/user.hpp"

// Forward declarations to avoid circular dependencies
namespace engine {
    class OrderQueue;
    class User;
}

namespace engine {
    static const std::uint16_t MAX_DENOMINATIONS{1000};

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
            std::uint64_t filledQuantity{0};
            std::uint64_t transactedValue{0};
    };
}

template<>
struct std::formatter<engine::OrderType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    
    template<typename FormatContext>
    auto format(const engine::OrderType& type, FormatContext& ctx) {
        switch (type) {
            case engine::OrderType::BUY:
                return std::format_to(ctx.out(), "BUY");
            case engine::OrderType::SELL:
                return std::format_to(ctx.out(), "SELL");
        }
    }
};

template<>
struct std::formatter<engine::OrderMode> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const engine::OrderMode& mode, FormatContext& ctx) {
        switch (mode) {
            case engine::OrderMode::MARKET:
                return std::format_to(ctx.out(), "MARKET");
            case engine::OrderMode::LIMIT:
                return std::format_to(ctx.out(), "LIMIT");
        }
    }
};