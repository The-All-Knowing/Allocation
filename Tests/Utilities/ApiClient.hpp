#pragma once

#include "Precompile.hpp"


namespace Allocation::Tests::ApiClient
{
    Poco::URI GetURI(const std::string& command);

    void PostToAddBatch(const std::string& ref, const std::string& sku, int qty,
        std::optional<std::chrono::year_month_day> eta = std::nullopt);

    std::string PostToAllocate(
        const std::string& orderid, const std::string& sku, int qty, bool expectSuccess = true);
}