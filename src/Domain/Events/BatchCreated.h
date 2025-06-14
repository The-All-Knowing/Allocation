#pragma once

#include "Precompile.h"
#include "IEvent.h"


namespace Allocation::Domain::Events
{

    struct BatchCreated final : public IEvent
    {
        BatchCreated(std::string ref, std::string sku, size_t qty, std::optional<std::chrono::year_month_day> eta = std::nullopt) :
            ref(std::move(ref)), sku(std::move(sku)), qty(qty), eta(std::move(eta))
        {};
        std::string Name() const override { return "BatchCreated"; };

        std::string ref;
        std::string sku;
        size_t qty;
        std::optional<std::chrono::year_month_day> eta;
    };
}