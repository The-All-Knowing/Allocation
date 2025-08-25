#pragma once

#include "AbstractCommand.hpp"


namespace Allocation::Domain::Commands
{
    struct Allocate final : public AbstractCommand
    {
        Allocate(std::string orderid, std::string sku, size_t qty)
            : orderid(std::move(orderid)), sku(std::move(sku)), qty(qty)
        {
        }

        [[nodiscard]] std::string Name() const override { return "Allocate"; };

        std::string orderid;
        std::string sku;
        size_t qty;
    };
}