#include "BatchMapper.hpp"

#include "Utilities/Common.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    using namespace Poco::Data::Keywords;

    BatchMapper::BatchMapper(const Poco::Data::Session& session) : _session(session) {}

    std::vector<Domain::Batch> BatchMapper::Find(std::string SKU) const
    {
        std::vector<Domain::Batch> result;
        Poco::Data::Statement select(_session);
        select << R"(
            SELECT id, reference, sku, _purchased_quantity, eta
            FROM allocation.batches
            WHERE sku = $1)",
            use(SKU);

        select.execute();
        Poco::Data::RecordSet rs(select);
        for (bool more = rs.moveFirst(); more; more = rs.moveNext())
        {
            int id = rs["id"].convert<int>();
            auto reference = rs["reference"].convert<std::string>();
            auto sku = rs["sku"].convert<std::string>();
            auto qty = rs["_purchased_quantity "].convert<size_t>();
            auto eta = Convert(rs["eta"].convert<Poco::DateTime>());

            Domain::Batch batch(reference, sku, qty, eta);

            for (const auto& order : GetAllocations(id))
                batch.Allocate(order);

            result.push_back(batch);
        }
        return result;
    }

    void BatchMapper::Delete(std::vector<std::string> batchRefs)
    {
        Poco::Data::Statement deleteOrderLines(_session);
        deleteOrderLines << R"(
            DELETE FROM allocation.order_lines p
            USING allocation.allocations l, allocation.batches k
            WHERE p.id = l.orderline_id
             AND l.batch_id = k.id
             AND k.reference = $1
        )",
            use(batchRefs), now;

        Poco::Data::Statement deleteBatch(_session);
        deleteBatch << R"(
            DELETE FROM allocation.batches
            WHERE reference = $1
        )",
            use(batchRefs), now;
    }

    void BatchMapper::Insert(const std::vector<Domain::Batch>& batches)
    {
        int id;
        std::string reference;
        std::string sku;
        Poco::Nullable<Poco::DateTime> eta;
        size_t qty;
        Poco::Data::Statement insertBatch(_session);
        insertBatch << R"(
            INSERT INTO allocation.batches (reference, sku, _purchased_quantity, eta)
            VALUES ($1, $2, $3, $4)
            RETURNING id
        )",
            use(reference), use(sku), use(qty), use(eta), into(id);

        for (const auto& batch : batches)
        {
            reference = batch.GetReference();
            sku = batch.GetSKU();
            eta = Convert(batch.GetETA());
            qty = batch.GetPurchasedQuantity();
            insertBatch.execute();

            auto orderLines = batch.GetAllocations();

            InsertOrderLines(orderLines, id);
        }
    }

    std::vector<Domain::OrderLine> BatchMapper::GetAllocations(int batchPk) const
    {
        std::vector<Domain::OrderLine> result;
        Poco::Data::Statement select(_session);
        select << R"(
            SELECT l.sku, l.qty, l.orderid
            FROM allocation.order_lines l
            JOIN allocation.allocations o ON l.id = o.orderline_id
            WHERE o.batch_id = $1)",
            Poco::Data::Keywords::use(batchPk);

        select.execute();
        Poco::Data::RecordSet rs(select);
        for (bool more = rs.moveFirst(); more; more = rs.moveNext())
        {
            auto sku = rs["sku"].convert<std::string>();
            auto qty = rs["qty"].convert<size_t>();
            auto orderid = rs["orderid"].convert<std::string>();

            result.emplace_back(Domain::OrderLine{orderid, sku, qty});
        }
        return result;
    }

    void BatchMapper::InsertOrderLines(const std::vector<Domain::OrderLine>& orders, int batchPk)
    {
        using sqlOrderLine = Poco::Tuple<std::string, int, std::string>;
        std::vector<sqlOrderLine> orderLines;
        std::vector<int> ids;
        Poco::Data::Statement insertLines(_session);
        insertLines << R"(
            INSERT INTO allocation.order_lines (sku, qty, orderid)
            VALUES ($, $, $)
            RETURNING id
        )",
            use(orderLines), into(ids);

        for (const auto& line : orders)
            orderLines.emplace_back(line.SKU, line.quantity, line.reference);
        insertLines.execute();

        std::vector<Poco::Tuple<int, int>> allocations;
        Poco::Data::Statement insertAllocations(_session);
        insertAllocations << R"(
            INSERT INTO allocation.allocations (orderline_id, batch_id)
            VALUES ($, $, $)
        )",
            use(allocations);
        for (int id : ids)
            allocations.emplace_back(id, batchPk);
        insertAllocations.execute();
    }
}