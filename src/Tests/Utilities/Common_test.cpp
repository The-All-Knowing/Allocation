#include "Common_test.hpp"


namespace Allocation::Tests
{
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty)
    {
        return {Domain::Batch("batch-001", SKU, batchQty),
            Domain::OrderLine("order-123", SKU, lineQty)};
    }

    std::string RandomSuffix()
    {
        return Poco::UUIDGenerator::defaultGenerator().createRandom().toString().substr(0, 6);
    }

    std::string RandomSku(const std::string& name) { return "sku-" + name + "-" + RandomSuffix(); }

    std::string RandomBatchRef(const std::string& name)
    {
        return "batch-" + name + "-" + RandomSuffix();
    }

    std::string RandomOrderId(const std::string& name)
    {
        return "order-" + name + "-" + RandomSuffix();
    }
}