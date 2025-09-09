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
        return std::make_shared<Domain::Product>(SKU, batches, version, false);
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
        return std::make_shared<Domain::Product>(SKU, batches, version, false);
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

        Poco::Data::Statement insert(_session);
        insert << R"(
            INSERT INTO allocation.products (sku, version_number)
            VALUES ($1, $2)
        )",
            use(sku), use(version), now;

        if (!batches.empty())
            _batchMapper.Insert(batches);
    }

    bool ProductMapper::Delete(Domain::ProductPtr product)
    {
        std::vector<std::string> deletedBatches;
        for (const auto& batch : product->GetBatches())
            deletedBatches.push_back(batch.GetReference());

        if (!deletedBatches.empty())
            _batchMapper.Delete(deletedBatches);

        auto SKU = product->GetSKU();
        int version = product->GetVersion();
        Poco::Data::Statement deleteProduct(_session);
        deleteProduct << R"(
            DELETE FROM allocation.products
            WHERE sku = $1 AND version_number = $2
        )",
            use(SKU), use(version);

        return deleteProduct.execute() > 0;
    }

    void ProductMapper::UpdateBatches(Domain::ProductPtr product)
    {
        auto changedBatchRefs = product->GetModifiedBatches();
        std::vector<Domain::Batch> changedBatches;

        for (const auto& ref : changedBatchRefs)
            if (auto batch = product->GetBatch(ref); batch.has_value())
                changedBatches.push_back(batch.value());

        if (!changedBatchRefs.empty())
            _batchMapper.Delete(changedBatchRefs);
        if (!changedBatches.empty())
            _batchMapper.Insert(changedBatches);
    }
}
