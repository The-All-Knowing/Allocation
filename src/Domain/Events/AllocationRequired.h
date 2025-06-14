#pragma once

#include "IEvent.h"


namespace Allocation::Domain::Events
{

    struct AllocationRequired final : public IEvent
    {
        AllocationRequired(std::string orderid, std::string sku, size_t qty) :
            orderid(std::move(orderid)), sku(std::move(sku)), qty(qty) 
        {}
        std::string Name() const override { return "AllocationRequired"; };

        std::string orderid;
        std::string sku;
        size_t qty;
    };
}