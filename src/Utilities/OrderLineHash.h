#pragma once

#include "Precompile.h"
#include "OrderLine.h"


namespace Allocation::Utilities
{

    struct OrderLineHash
    {
        size_t operator() (const Domain::OrderLine& line) const
        {
            size_t h1 = std::hash<std::string>{}(line.GetOrderReference());
            size_t h2 = std::hash<size_t>{}(line.GetQuantity());
            size_t h3 = std::hash<std::string>{}(line.GetSKU());
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

}