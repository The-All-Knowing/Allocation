#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    struct ChangeBatchQuantity final : public AbstractCommand
    {
        ChangeBatchQuantity(std::string ref, size_t qty) : ref(std::move(ref)), qty(qty) {}
        [[nodiscard]] std::string Name() const override { return "ChangeBatchQuantity"; };

        std::string ref;
        size_t qty;
    };
}