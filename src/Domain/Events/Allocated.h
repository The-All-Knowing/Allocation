#pragma once

#include "AbstractEvent.h"


namespace Allocation::Domain::Events
{

    struct Allocated final : public AbstractEvent
    {
        Allocated(std::string SKU, std::string batchref, size_t qty) : 
            SKU(std::move(SKU)), batchref(std::move(batchref)), qty(qty)
        {};

        [[nodiscard]] std::string Name() const override { return "Allocated"; };

        std::string SKU;
        size_t qty;
        std::string batchref;
    };
}