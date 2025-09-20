#include "ProductMapper.hpp"

#include "BatchMapper.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    using namespace Poco::Data::Keywords;

    ProductMapper::ProductMapper(const Poco::Data::Session& session)
        : _session(session), _batchMapper(_session)
    {
    }

    Domain::ProductPtr ProductMapper::FindBySKU(const std::string& sku) const
    {
        if (sku.empty())
            return nullptr;

        int version;
        Poco::Data::Statement selectProduct(_session);
        selectProduct << R"(
            SELECT version_number
            FROM allocation.products
            WHERE sku = $1
        )",
            useRef(sku), into(version);

        bool found = selectProduct.execute() > 0;
        if (!found)
            return nullptr;

        auto batches = _batchMapper.Find(sku);
        return std::make_shared<Domain::Product>(sku, batches, version, false);
    }

    Domain::ProductPtr ProductMapper::FindByBatchRef(const std::string& ref) const
    {
        if (ref.empty())
            return nullptr;

        std::string sku;
        int version;
        Poco::Data::Statement selectProduct(_session);
        selectProduct << R"(
            SELECT p.sku, p.version_number
            FROM allocation.products p
            JOIN allocation.batches b ON p.sku = b.sku
            WHERE b.reference = $1
            LIMIT 1
        )",
            useRef(ref), into(sku), into(version);

        bool found = selectProduct.execute() > 0;
        if (!found)
            return nullptr;

        auto batches = _batchMapper.Find(sku);
        return std::make_shared<Domain::Product>(sku, batches, version, false);
    }

    bool ProductMapper::Update(Domain::ProductPtr product, int oldVersion)
    {
        if (!product)
            throw std::invalid_argument("Product is nullptr");

        auto changedBatchRefs = product->GetModifiedBatches();
        std::vector<Domain::Batch> updateBatches;
        for (const auto& ref : changedBatchRefs)
            if (auto batch = product->GetBatch(ref); batch.has_value())
                updateBatches.push_back(batch.value());
        UpdateBatches(updateBatches, changedBatchRefs);

        auto sku = product->GetSKU();
        int newVersion = product->GetVersion();
        Poco::Data::Statement updateProduct(_session);
        updateProduct << R"(
            UPDATE allocation.products
            SET version_number = $1
            WHERE sku = $2 AND version_number = $3
        )",
            use(newVersion), use(sku), use(oldVersion);

        return updateProduct.execute() > 0;
    }

    void ProductMapper::Insert(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("Product is nullptr");

        auto sku = product->GetSKU();
        int version = product->GetVersion();
        auto batches = product->GetBatches();

        Poco::Data::Statement insertProduct(_session);
        insertProduct << R"(
            INSERT INTO allocation.products (sku, version_number)
            VALUES ($1, $2)
        )",
            use(sku), use(version), now;

        if (!batches.empty())
            _batchMapper.Insert(batches);
    }

    bool ProductMapper::Delete(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("Product is nullptr");

        std::vector<std::string> batchRefs;
        for (const auto& batch : product->GetBatches())
            batchRefs.push_back(batch.GetReference());

        if (!batchRefs.empty())
            _batchMapper.Delete(batchRefs);

        auto sku = product->GetSKU();
        int version = product->GetVersion();
        Poco::Data::Statement deleteProduct(_session);
        deleteProduct << R"(
            DELETE FROM allocation.products
            WHERE sku = $1 AND version_number = $2
        )",
            use(sku), use(version);

        return deleteProduct.execute() > 0;
    }

    void ProductMapper::UpdateBatches(
        const std::vector<Domain::Batch>& batches, const std::vector<std::string>& batchRefs)
    {
        if (!batchRefs.empty())
            _batchMapper.Delete(batchRefs);
        if (!batches.empty())
            _batchMapper.Insert(batches);
    }
}
