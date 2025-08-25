#pragma once

#include "Precompile.hpp"


namespace Allocation::Domain
{
    struct OrderLine
    {
        std::string reference;
        std::string SKU;
        size_t quantity;

        auto operator<=>(const OrderLine&) const = default;
    };
}

template <>
struct std::hash<Allocation::Domain::OrderLine>
{
    std::size_t operator()(const Allocation::Domain::OrderLine& line) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(line.reference);
        std::size_t h2 = std::hash<std::string>{}(line.SKU);
        std::size_t h3 = std::hash<size_t>{}(line.quantity);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};