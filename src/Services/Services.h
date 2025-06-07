#pragma once

#include "Precompile.h"
#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services
{
    void AddBatch(
        Domain::IUnitOfWork& uow,
        std::string_view ref, std::string_view SKU, size_t qty,
        std::optional<std::chrono::year_month_day> ETA = std::nullopt);

    std::string Allocate(
        Domain::IUnitOfWork& uow, std::string_view orderid,
        std::string_view SKU, size_t qty);
}