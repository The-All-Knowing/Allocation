#pragma once

#include "IEvent.h"


namespace Allocation::Domain::Events
{

    struct OutOfStock final : public IEvent
    {
        OutOfStock(std::string SKU): SKU(std::move(SKU)) {};

        std::string Name() const override { return "OutOfStock"; };
        std::string SKU;
    };
}