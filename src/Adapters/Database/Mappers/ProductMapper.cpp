#include "ProductMapper.hpp"

#include "BatchMapper.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    using namespace Poco::Data::Keywords;

    ProductMapper::ProductMapper(const Poco::Data::Session& session)
        : _session(session), _batchMapper(_session)
    {
    }

    Domain::ProductPtr ProductMapper::FindBySKU(std::string SKU) const
    {
        int version = 0;
        Poco::Data::Statement select(_session);
        select << R"(
            SELECT version_number
            FROM allocation.products
            WHERE sku = $1
        )",
            use(SKU), into(version);

        bool found = select.execute() > 0;
        if (!found)
            return nullptr;

        auto batches = _batchMapper.Find(SKU);
        return std::make_shared<Domain::Product>(SKU, batches, version);
    }

    Domain::ProductPtr ProductMapper::FindByBatchRef(std::string ref) const
    {
        std::string SKU;
        int version = 0;

        Poco::Data::Statement select(_session);
        select << R"(
            SELECT p.sku, p.version_number
            FROM allocation.products p
            JOIN allocation.batches b ON p.sku = b.sku
            WHERE b.reference = $1
            LIMIT 1
        )",
            use(ref), into(SKU), into(version);

        bool found = select.execute() > 0;
        if (!found)
            return nullptr;

        auto batches = _batchMapper.Find(SKU);
        return std::make_shared<Domain::Product>(SKU, batches, version);
    }

    bool ProductMapper::Update(Domain::ProductPtr product, int oldVersion)
    {   
        UpdateBatches(product);

        auto sku = product->GetSKU();
        int newVersion = product->GetVersion();
        Poco::Data::Statement update(_session);
        update << R"(
            UPDATE allocation.products
            SET version_number = $1
            WHERE sku = $2 AND version_number = $3
        )",
            use(newVersion), use(sku), use(oldVersion);

        return update.execute() > 0;
    }

    void ProductMapper::Insert(Domain::ProductPtr product)
    {
        auto sku = product->GetSKU();
        int version = product->GetVersion();
        auto batches = product->GetBatches();

        _session << R"(
            INSERT INTO allocation.products (sku, version_number)
            VALUES ($1, $2)
        )",
            use(sku), use(version), now;

        _batchMapper.Insert(batches);
    }

    void ProductMapper::UpdateBatches(Domain::ProductPtr product)
    {
        auto modifiedBatches = product->GetModifiedBatches();
        if (modifiedBatches.empty())
            return;

        std::vector<Domain::Batch> newBatches;
        newBatches.reserve(modifiedBatches.size());

        for (const auto& ref : modifiedBatches)
            if (auto batch = product->GetBatch(ref); batch.has_value())
                newBatches.push_back(batch.value());

        _batchMapper.Delete(modifiedBatches);
        if (!newBatches.empty())
            _batchMapper.Insert(newBatches);
    }
}
