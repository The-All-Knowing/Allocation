#include "ProductMapper.hpp"

#include "BatchMapper.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    ProductMapper::ProductMapper(Poco::Data::Session& session) : _session(session) {}

    bool ProductMapper::IsExists(std::string SKU)
    {
        Poco::Nullable<int> dummy;
        int rowsAffected = 0;
        _session << "SELECT 1 FROM public.products WHERE sku = $1",
            Poco::Data::Keywords::into(dummy), Poco::Data::Keywords::use(SKU),
            Poco::Data::Keywords::now;
        return !dummy.isNull();
    }

    std::shared_ptr<Domain::Product> ProductMapper::FindBySKU(std::string SKU)
    {
        BatchMapper _batchMapper(_session);
        std::shared_ptr<Domain::Product> result;

        Poco::Nullable<int> version;
        _session << R"(SELECT version_number FROM public.products WHERE sku = $1)",
            Poco::Data::Keywords::into(version), Poco::Data::Keywords::use(SKU),
            Poco::Data::Keywords::now;

        if (version.isNull())
            return result;

        auto batches = _batchMapper.GetBySKU(SKU);
        result = std::make_shared<Domain::Product>(SKU, batches, version.value());
        return result;
    }

    std::shared_ptr<Domain::Product> ProductMapper::FindByBatchRef(std::string ref)
    {
        BatchMapper _batchMapper(_session);
        std::shared_ptr<Domain::Product> result;

        std::string SKU;
        int version;

        Poco::Data::Statement select(_session);
        select << R"(
            SELECT public.products.sku, public.products.version_number FROM public.products
            JOIN public.batches ON public.products.sku = public.batches.sku
            WHERE public.batches.reference = $1)",
            Poco::Data::Keywords::use(ref), Poco::Data::Keywords::into(SKU),
            Poco::Data::Keywords::into(version), Poco::Data::Keywords::range(0, 1);

        while (!select.done())
        {
            select.execute();
            if (!SKU.empty())
            {
                auto batches = _batchMapper.GetBySKU(SKU);
                result = std::make_shared<Domain::Product>(SKU, batches, version);
            }
        }

        return result;
    }

    void ProductMapper::Update(std::shared_ptr<Domain::Product> product)
    {
        BatchMapper _batchMapper(_session);
        auto batches = product->GetBatches();

        _batchMapper.Update(batches, product->GetSKU());
    }

    void ProductMapper::Insert(std::shared_ptr<Domain::Product> product)
    {
        BatchMapper _batchMapper(_session);

        std::string sku = product->GetSKU();
        int version = product->GetVersion();

        _session << R"(INSERT INTO public.products (sku, version_number) VALUES ($1, $2))",
            Poco::Data::Keywords::use(sku), Poco::Data::Keywords::use(version),
            Poco::Data::Keywords::now;

        _batchMapper.Insert(product->GetBatches());
    }

    bool ProductMapper::UpdateVersion(std::string SKU, size_t oldVersion, size_t newVersion)
    {
        Poco::Nullable<int> actualVersion;
        _session << "UPDATE public.products SET version_number = $1 WHERE sku = $2 AND "
                    "version_number = $3 RETURNING version_number",
            Poco::Data::Keywords::into(actualVersion), Poco::Data::Keywords::use(newVersion),
            Poco::Data::Keywords::use(SKU), Poco::Data::Keywords::use(oldVersion),
            Poco::Data::Keywords::now;

        if (actualVersion.isNull() || (actualVersion.value() != newVersion))
            return false;

        return true;
    }
}