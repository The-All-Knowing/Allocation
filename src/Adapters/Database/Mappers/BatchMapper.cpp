#include "Mappers/BatchMapper.h"
#include "Mappers/OrderLineMapper.h"
#include "Utilities/Common.h"


namespace Allocation::Adapters::Database::Mapper
{
    BatchMapper::BatchMapper(Poco::Data::Session& session): _session(session)
    {}

    std::optional<int> BatchMapper::GetBatchId(std::string reference)
    {
        Poco::Nullable<int> id;
        _session << R"(SELECT id FROM batches WHERE reference = ?)",
            Poco::Data::Keywords::into(id),
            Poco::Data::Keywords::use(reference),
            Poco::Data::Keywords::now;

        if(id.isNull())
            return std::optional<int>();
        
        return id;
    }

    std::vector<Domain::Batch> BatchMapper::GetAll()
    {
        OrderLineMapper ordersMapper(_session);
        std::vector<Domain::Batch> result;

        Poco::Data::Statement select(_session);
        select << R"(SELECT id, reference, sku, _purchased_quantity, eta FROM batches)",
                Poco::Data::Keywords::now;

        Poco::Data::RecordSet rs(select);
        bool more = rs.moveFirst();
        while(more)
        {
            size_t id = rs["id"].convert<size_t>();
            std::string reference = rs["reference"].convert<std::string>();
            std::string sku = rs["sku"].convert<std::string>();
            size_t qty = rs["_purchased_quantity"].convert<size_t>();

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

    void BatchMapper::Insert(const Domain::Batch& batch)
    {
        OrderLineMapper ordersMapper(_session);
        const auto& allocations = batch.GetAllocations();

        size_t totalQty = batch.GetAvailableQuantity();
        for (const auto& order : allocations)
            totalQty+= order.GetQuantity(); 

        std::string reference = batch.GetReference();
        std::string sku = batch.GetSKU();

        Poco::Nullable<Poco::DateTime> pocoEta;
        if (auto eta = batch.GetETA(); eta.has_value())
            pocoEta = Convert(eta.value());

        _session << R"(INSERT INTO batches (reference, sku, _purchased_quantity, eta) VALUES (?, ?, ?, ?))",
            Poco::Data::Keywords::use(reference),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::use(totalQty),
            Poco::Data::Keywords::use(pocoEta),
            Poco::Data::Keywords::now;

        size_t batchId = 0;
        _session << R"(
            SELECT id FROM batches WHERE reference = ? AND sku = ?
            ORDER BY id DESC LIMIT 1)",
            Poco::Data::Keywords::into(batchId),
            Poco::Data::Keywords::use(reference),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::now;

        ordersMapper.Insert(batchId, allocations);
    }

    void BatchMapper::Update(int id, const Domain::Batch& batch)
    {
        const auto& allocations = batch.GetAllocations();
        size_t purchasedQty = batch.GetAvailableQuantity();
        for (const auto& order : allocations)
            purchasedQty += order.GetQuantity();

        std::string sku = batch.GetSKU();

        Poco::Nullable<Poco::DateTime> pocoEta;
        if (auto eta = batch.GetETA(); eta.has_value())
            pocoEta = Convert(eta.value());

        _session << R"(
            UPDATE batches
            SET sku = ?, _purchased_quantity = ?, eta = ?
            WHERE id = ?
        )",
        Poco::Data::Keywords::use(sku),
        Poco::Data::Keywords::use(purchasedQty),
        Poco::Data::Keywords::use(pocoEta),
        Poco::Data::Keywords::use(id),
        Poco::Data::Keywords::now;

        UpdateAllocations(id, allocations);
    }

    std::optional<Domain::Batch> BatchMapper::FindByReference(std::string reference)
    {
        OrderLineMapper ordersMapper(_session);
        std::optional<Domain::Batch> result;

        Poco::Data::Statement select(_session);
        select << R"(SELECT id, sku, _purchased_quantity, eta FROM batches WHERE reference = ?)", 
                Poco::Data::Keywords::use(reference),
                Poco::Data::Keywords::now;

        Poco::Data::RecordSet rs(select);
        if (!rs.moveFirst())
            return std::nullopt;

        size_t id = rs["id"].convert<size_t>();
        std::string sku = rs["sku"].convert<std::string>();
        size_t qty = rs["_purchased_quantity"].convert<size_t>();

        std::optional<std::chrono::year_month_day> eta;
        if (!rs["eta"].isEmpty())
            eta = Convert(rs["eta"].convert<Poco::DateTime>());

        result = Domain::Batch(reference, sku, qty, eta);

        for (const auto& order : ordersMapper.FindByBatchId(id))
            result->Allocate(order);

        return result;
    }

    void BatchMapper::UpdateAllocations(size_t batchId, const std::vector<Domain::OrderLine>& orders)
    {
        OrderLineMapper ordersMapper(_session);

        ordersMapper.RemoveByBatchId(batchId);
        if (!orders.empty())
            ordersMapper.Insert(batchId, orders);    
    }
}