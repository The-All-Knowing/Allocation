#include "BatchMapper.hpp"

#include "Utilities/Common.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    using namespace Poco::Data::Keywords;

    BatchMapper::BatchMapper(const Poco::Data::Session& session) : _session(session) {}

    std::vector<Domain::Batch> BatchMapper::Find(const std::string& sku) const
    {
        std::vector<Domain::Batch> result;
        if (sku.empty())
            return result;

        Poco::Data::Statement findBatches(_session);
        findBatches << R"(
            SELECT id, reference, sku, _purchased_quantity, eta
            FROM allocation.batches
            WHERE sku = $1)",
            useRef(sku);
        findBatches.execute();

        Poco::Data::RecordSet rs(findBatches);
        for (bool more = rs.moveFirst(); more; more = rs.moveNext())
        {
            int id = rs["id"].convert<int>();
            auto reference = rs["reference"].convert<std::string>();
            auto sku = rs["sku"].convert<std::string>();
            auto qty = rs["_purchased_quantity"].convert<size_t>();
            std::optional<std::chrono::year_month_day> eta;
            if (!rs["eta"].isEmpty())
                eta = Convert(rs["eta"].convert<Poco::DateTime>());
            Domain::Batch batch(reference, sku, qty, eta);

            for (const auto& order : FindOrderLines(id))
                batch.Allocate(order);

            result.push_back(batch);
        }
        return result;
    }

    void BatchMapper::Delete(std::vector<std::string> batchRefs)
    {
        if (batchRefs.empty())
            return;

        DeleteAllOrderLines(batchRefs);

        Poco::Data::Statement deleteBatches(_session);
        deleteBatches << R"(
            DELETE FROM allocation.batches
            WHERE reference IN ($1)
        )",
            use(batchRefs), now;
    }

    void BatchMapper::Insert(const std::vector<Domain::Batch>& batches)
    {
        if (batches.empty())
            return;

        int batchPk;
        std::string reference;
        std::string sku;
        Poco::Nullable<Poco::DateTime> eta;
        size_t qty;
        Poco::Data::Statement insertBatches(_session);
        insertBatches << R"(
            INSERT INTO allocation.batches (reference, sku, _purchased_quantity, eta)
            VALUES ($1, $2, $3, $4)
            RETURNING id
        )",
            use(reference), use(sku), use(qty), use(eta), into(batchPk);

        for (const auto& batch : batches)
        {
            reference = batch.GetReference();
            sku = batch.GetSKU();
            eta = Convert(batch.GetETA());
            qty = batch.GetPurchasedQuantity();
            insertBatches.execute();

            auto orderLines = batch.GetAllocations();
            if (!orderLines.empty())
                InsertOrderLines(orderLines, batchPk);
        }
    }

    std::vector<Domain::OrderLine> BatchMapper::FindOrderLines(int batchPk) const
    {
        std::vector<Domain::OrderLine> result;
        Poco::Data::Statement selectOrderLines(_session);
        selectOrderLines << R"(
            SELECT l.sku, l.qty, l.orderid
            FROM allocation.order_lines l
            JOIN allocation.allocations o ON l.id = o.orderline_id
            WHERE o.batch_id = $1)",
            use(batchPk), now;

        Poco::Data::RecordSet rs(selectOrderLines);
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
        if (orders.empty())
            return;

        using sqlOrderLine = Poco::Tuple<std::string, int, std::string>;
        std::vector<sqlOrderLine> orderLines;
        std::vector<int> ids;

        for (const auto& line : orders)
            orderLines.emplace_back(line.sku, line.quantity, line.reference);

        Poco::Data::Statement insertOrderLines(_session);
        insertOrderLines << R"(
            INSERT INTO allocation.order_lines (sku, qty, orderid)
            VALUES ($1, $2, $3)
            RETURNING id
        )",
            use(orderLines), into(ids), now;

        std::vector<Poco::Tuple<int, int>> allocations;
        for (int id : ids)
            allocations.emplace_back(id, batchPk);

        Poco::Data::Statement insertAllocations(_session);
        insertAllocations << R"(
            INSERT INTO allocation.allocations (orderline_id, batch_id)
            VALUES ($1, $2)
        )",
            use(allocations), now;
    }

    void BatchMapper::DeleteAllOrderLines(std::vector<std::string> batchRefs)
    {
        if (batchRefs.empty())
            return;

        std::vector<int> removedOrderLinesId;
        Poco::Data::Statement deleteAllocations(_session);
        deleteAllocations << R"(
            DELETE FROM allocation.allocations l
            USING allocation.batches b
            WHERE l.batch_id = b.id
             AND b.reference IN ($1)
            RETURNING l.orderline_id
        )",
            use(batchRefs), into(removedOrderLinesId), now;

        if (removedOrderLinesId.empty())
            return;

        Poco::Data::Statement deleteOrderLines(_session);
        deleteOrderLines << R"(
            DELETE FROM allocation.order_lines
            WHERE id IN ($1)
        )",
            use(removedOrderLinesId), now;
    }
}