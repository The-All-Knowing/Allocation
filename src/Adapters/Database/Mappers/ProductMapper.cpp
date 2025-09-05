#include "ProductMapper.hpp"

#include "BatchMapper.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    ProductMapper::ProductMapper(const Poco::Data::Session& session) : _session(session) {}

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

    bool ProductMapper::Update(Domain::ProductPtr product, int oldVersion)
    {
        BatchMapper _batchMapper(_session);
        auto batches = product.GetBatches();

        _batchMapper.Update(batches, product.GetSKU());
    }

    void ProductMapper::Insert(Domain::ProductPtr product)
    {
        BatchMapper _batchMapper(_session);

        std::string sku = product.GetSKU();
        int version = product.GetVersion();

        _session << R"(INSERT INTO public.products (sku, version_number) VALUES ($1, $2))",
            Poco::Data::Keywords::use(sku), Poco::Data::Keywords::use(version),
            Poco::Data::Keywords::now;

        _batchMapper.Insert(product.GetBatches());
    }
}