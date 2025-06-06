#pragma once

#include "Precompile.h"
#include "Forwards.h"
#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services
{
    void AddBatch(Domain::IUnitOfWork& uow,
        std::string ref, std::string sku, int qty,
        std::optional<std::chrono::year_month_day> ETA = std::nullopt);

    std::string Allocate(Domain::IUnitOfWork& uow,
        std::string orderid, std::string sku, int qty);
}