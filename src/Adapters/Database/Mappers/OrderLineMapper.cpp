#pragma once

#include "Mappers/OrderLineMapper.h"


namespace Allocation::Adapters::Database::Mapper
{
    OrderLineMapper::OrderLineMapper(Poco::Data::Session& session): _session(session)
    {}

    std::vector<Domain::OrderLine> OrderLineMapper::FindByBatchId(int batchId)
    {   
        Poco::Data::Statement select(_session);
        select << R"(SELECT sku, qty, orderid FROM public.order_lines
                   JOIN public.allocations ON allocations.id = order_lines.id
                   WHERE allocations.batch_id = $1)",
                    Poco::Data::Keywords::use(batchId),
                    Poco::Data::Keywords::now;

        Poco::Data::RecordSet rs(select);
        std::vector<Domain::OrderLine> result;
        bool more = rs.moveFirst();
        while (more)
        {
            std::string sku = rs["sku"].convert<std::string>();
            size_t qty = rs["qty"].convert<size_t>();
            std::string orderid = rs["orderid"].convert<std::string>();

            result.emplace_back(orderid, sku, qty);
            more = rs.moveNext();
        }

        return result;
    }

    void OrderLineMapper::Insert(int batchId, const std::vector<Domain::OrderLine>& orders)
    {
        for (const auto& order : orders)
        {
            std::string sku = order.SKU;
            int qty = static_cast<int>(order.quantity);
            std::string orderid = order.reference;

            int orderlineId = 0;
            _session << R"(
                INSERT INTO public.order_lines (sku, qty, orderid)
                VALUES ($1, $2, $3)
                RETURNING id
            )",
                Poco::Data::Keywords::into(orderlineId),
                Poco::Data::Keywords::use(sku),
                Poco::Data::Keywords::use(qty),
                Poco::Data::Keywords::use(orderid),
                Poco::Data::Keywords::now;

            _session << R"(
                INSERT INTO public.allocations (orderline_id, batch_id)
                VALUES ($1, $2)
            )",
                Poco::Data::Keywords::use(orderlineId),
                Poco::Data::Keywords::use(batchId),
                Poco::Data::Keywords::now;
        }
    }

    void OrderLineMapper::RemoveByBatchesId(std::vector<int> batchesId)
    {
        _session << R"(
            DELETE FROM public.allocations WHERE batch_id IN ($1)
        )",
        Poco::Data::Keywords::use(batchesId),
        Poco::Data::Keywords::now;

        _session << R"(
            DELETE FROM public.order_lines 
            WHERE id NOT IN (
                SELECT orderline_id FROM public.allocations
            )
        )",
        Poco::Data::Keywords::now;
    }
}