#pragma once

#include "Mappers/OrderLineMapper.h"


namespace Allocation::Adapters::Database::Mapper
{
    OrderLineMapper::OrderLineMapper(Poco::Data::Session& session): _session(session)
    {}

    std::optional<Domain::OrderLine> OrderLineMapper::FindById(size_t id)
    {
        return std::optional<Domain::OrderLine>();
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
        
    void OrderLineMapper::Update(const Domain::OrderLine& entity)
    {

    }

    void OrderLineMapper::Remove(size_t id)
    {

    }
}