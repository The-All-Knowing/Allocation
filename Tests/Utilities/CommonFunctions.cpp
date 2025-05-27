#include "CommonFunctions.h"


namespace Allocation::Tests
{
    std::chrono::year_month_day GetCurrentDate()
    {
        const std::chrono::time_point now{std::chrono::system_clock::now()};
        return std::chrono::floor<std::chrono::days>(now);
    }

    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty)
    {
        return {Domain::Batch("batch-001", SKU, batchQty, GetCurrentDate()),
                Domain::OrderLine("order-123", SKU, lineQty)};
    }

}