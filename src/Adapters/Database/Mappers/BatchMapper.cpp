#include "Mappers/BatchMapper.h"
#include "Mappers/OrderLineMapper.h"
#include "Utilities/Common.h"


namespace Allocation::Adapters::Database::Mapper
{
    BatchMapper::BatchMapper(Poco::Data::Session& session): _session(session)
    {}

    std::vector<Domain::Batch> BatchMapper::GetBySKU(const std::string& SKU)
    {
        OrderLineMapper ordersMapper(_session);
        std::vector<Domain::Batch> result;

        auto id = GetIdBatches(SKU);

        if(id.empty())
            return result;

        Poco::Data::Statement select(_session);
        select << R"(SELECT id, reference, sku, purchased_quantity, eta
                    FROM public.batches
                    WHERE id IN ($1))",
                Poco::Data::Keywords::use(id),
                Poco::Data::Keywords::now;

        Poco::Data::RecordSet rs(select);
        bool more = rs.moveFirst();

        while(more)
        {
            size_t id = rs["id"].convert<size_t>();
            std::string reference = rs["reference"].convert<std::string>();
            std::string sku = rs["sku"].convert<std::string>();
            size_t qty = rs["purchased_quantity"].convert<size_t>();

            std::optional<std::chrono::year_month_day> eta;
            if (!rs["eta"].isEmpty())
                eta = Convert(rs["eta"].convert<Poco::DateTime>());

            Domain::Batch batch(reference, sku, qty, eta);
            for (const auto& order : ordersMapper.FindByBatchId(id))
                batch.Allocate(order);
            
            result.push_back(batch);
            more = rs.moveNext();
        }

        return result;
    }

    void BatchMapper::Update(const std::vector<Domain::Batch>& batches, const std::string& SKU)
    {
        DeleteBatches(SKU);
        if (!batches.empty())
            Insert(batches);
    }

    void BatchMapper::DeleteBatches(std::string SKU)
    {
        if (auto id = GetIdBatches(SKU); !id.empty())
        {
            OrderLineMapper ordersMapper(_session);
            ordersMapper.RemoveByBatchesId(id);
            
            _session << R"(DELETE FROM public.batches WHERE sku IN ($1))",
                            Poco::Data::Keywords::use(id),
                            Poco::Data::Keywords::now;
        }
    }

    int BatchMapper::GetIdBatch(std::string reference)
    {
        int batchId = 0;

        _session << R"(
            SELECT id FROM public.batches WHERE reference = $1
            ORDER BY id DESC LIMIT 1)",
            Poco::Data::Keywords::into(batchId),
            Poco::Data::Keywords::use(reference),
            Poco::Data::Keywords::now;

        return batchId;
    }

    std::vector<int> BatchMapper::GetIdBatches(std::string SKU)
    {
        std::vector<int> batchesId;

        _session << R"(SELECT id FROM public.batches WHERE sku = $1)",
                        Poco::Data::Keywords::into(batchesId),
                        Poco::Data::Keywords::use(SKU),
                        Poco::Data::Keywords::now;

        return batchesId;
    }

    void BatchMapper::Insert(const std::vector<Domain::Batch>& batches)
    {
        OrderLineMapper ordersMapper(_session);

        for (auto& batch : batches)
        {
            const auto& allocations = batch.GetAllocations();

            size_t totalQty = batch.GetAvailableQuantity();
            for (const auto& order : allocations)
                totalQty+= order.quantity; 

            std::string reference{batch.GetReference()};
            std::string sku{batch.GetSKU()};

            Poco::Nullable<Poco::DateTime> pocoEta;
            if (auto eta = batch.GetETA(); eta.has_value())
                pocoEta = Convert(eta.value());

            _session << R"(INSERT INTO public.batches (reference, sku, purchased_quantity, eta)
                            VALUES ($1, $2, $3, $4))",
                Poco::Data::Keywords::use(reference),
                Poco::Data::Keywords::use(sku),
                Poco::Data::Keywords::use(totalQty),
                Poco::Data::Keywords::use(pocoEta),
                Poco::Data::Keywords::now;

            int batchId = GetIdBatch(reference);
            ordersMapper.Insert(batchId, allocations);
        }
    }
}