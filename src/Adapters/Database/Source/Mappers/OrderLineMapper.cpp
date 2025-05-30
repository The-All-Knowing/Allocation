#pragma once

#include "Mappers/OrderLineMapper.h"


namespace Allocation::Adapters::Database::Mapper
{
    OrderLineMapper::OrderLineMapper(Poco::Data::Session& session): _session(session)
    {}

    std::vector<Domain::OrderLine> OrderLineMapper::FindByBatchId(size_t batchId)
    {   
        Poco::Data::Statement select(_session);
        select << R"(SELECT sku, qty, orderid FROM order_lines
                   JOIN allocations ON allocations.id = order_lines.id
                   WHERE allocations.batch_id = ?)",
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

    std::vector<Domain::OrderLine> OrderLineMapper::FindAll()
    {
        Poco::Data::Statement select(_session);
        select << "SELECT sku, qty, orderid FROM order_lines", 
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

    void OrderLineMapper::Insert(size_t batchId, const std::vector<Domain::OrderLine>& orders)
    {
        for (const auto& order : orders)
        {
            std::string sku = order.GetSKU();
            int qty = static_cast<int>(order.GetQuantity());
            std::string orderid = order.GetOrderReference();

            _session << R"(
                INSERT INTO order_lines (sku, qty, orderid)
                VALUES (?, ?, ?)
            )",
                Poco::Data::Keywords::use(sku),
                Poco::Data::Keywords::use(qty),
                Poco::Data::Keywords::use(orderid),
                Poco::Data::Keywords::now;

            int orderlineId = 0;
            _session << R"(
                SELECT id FROM order_lines
                WHERE orderid = ? AND sku = ?
                ORDER BY id DESC LIMIT 1
            )",
                Poco::Data::Keywords::into(orderlineId),
                Poco::Data::Keywords::use(orderid),
                Poco::Data::Keywords::use(sku),
                Poco::Data::Keywords::now;

            _session << R"(
                INSERT INTO allocations (orderline_id, batch_id)
                VALUES (?, ?)
            )",
                Poco::Data::Keywords::use(orderlineId),
                Poco::Data::Keywords::use(batchId),
                Poco::Data::Keywords::now;
        }
    }

    void OrderLineMapper::Insert(const Domain::OrderLine& orderLine)
    {
        auto sku = orderLine.GetSKU();
        auto orderRef = orderLine.GetOrderReference();
        auto qty = static_cast<Poco::UInt64>(orderLine.GetQuantity());

        _session << R"(INSERT INTO order_lines (sku, qty, orderid) VALUES(?, ?, ?))",
                        Poco::Data::Keywords::use(sku),
                        Poco::Data::Keywords::use(qty),
                        Poco::Data::Keywords::use(orderRef),
                        Poco::Data::Keywords::now;
    }
        
    void OrderLineMapper::RemoveByBatchId(size_t batchId)
    {
        _session << R"(
            DELETE FROM allocations WHERE batch_id = ?
        )",
        Poco::Data::Keywords::use(batchId),
        Poco::Data::Keywords::now;

        _session << R"(
            DELETE FROM order_lines 
            WHERE id NOT IN (
                SELECT orderline_id FROM allocations
            )
        )",
        Poco::Data::Keywords::now;
    }

}