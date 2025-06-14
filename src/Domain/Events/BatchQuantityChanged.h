#pragma once

#include "IEvent.h"


namespace Allocation::Domain::Events
{

    struct BatchQuantityChanged final : public IEvent
    {
        BatchQuantityChanged(std::string ref, size_t qty) : ref(std::move(ref)), qty(qty) 
        {}
        std::string Name() const override { return "BatchQuantityChanged"; };
        
        std::string ref;
        size_t qty;
    };
}