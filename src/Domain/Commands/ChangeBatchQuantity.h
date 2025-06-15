#pragma once

#include "AbstractCommand.h"


namespace Allocation::Domain::Commands
{

    struct ChangeBatchQuantity final : public AbstractCommand
    {
        ChangeBatchQuantity(std::string ref, size_t qty) : ref(std::move(ref)), qty(qty) 
        {}
        std::string Name() const override { return "ChangeBatchQuantity"; };
        
        std::string ref;
        size_t qty;
    };
}