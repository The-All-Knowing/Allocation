#pragma once

#include "Precompile.h"
#include "Forwards.h"


namespace Allocation::Services
{
    void AddBatch(IRepositoryPtr repo,
        Poco::Data::Session& session,
        std::string ref, std::string sku, int qty,
        std::optional<std::chrono::year_month_day> ETA = std::nullopt);

    std::string Allocate(IRepositoryPtr repo,
        Poco::Data::Session& session, 
        std::string orderid, std::string sku, int qty);
}