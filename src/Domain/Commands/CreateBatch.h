#pragma once

#include "AbstractCommand.h"


namespace Allocation::Domain::Commands
{

    struct CreateBatch final : public AbstractCommand
    {
        CreateBatch(std::string ref, std::string sku, size_t qty, std::optional<std::chrono::year_month_day> eta = std::nullopt) :
            ref(std::move(ref)), sku(std::move(sku)), qty(qty), eta(std::move(eta))
        {};
        std::string Name() const override { return "CreateBatch"; };

        std::string ref;
        std::string sku;
        size_t qty;
        std::optional<std::chrono::year_month_day> eta;
    };
}