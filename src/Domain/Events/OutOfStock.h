#pragma once

#include "AbstractEvent.h"


namespace Allocation::Domain::Events
{

    struct OutOfStock final : public AbstractEvent
    {
        OutOfStock(std::string SKU): SKU(std::move(SKU)) {};

        [[nodiscard]] std::string Name() const override { return "OutOfStock"; };
        std::string SKU;
    };
}