#pragma once

#include "Precompile.h"
#include "OrderLine.h"


namespace Allocation::Adapters::Database::Mapper
{
    class OrderLineMapper
    {
    public:
        OrderLineMapper(Poco::Data::Session& session);
        std::vector<Domain::OrderLine> FindByBatchId(size_t batchId);
        std::vector<Domain::OrderLine> FindAll();
        void Insert(const Domain::OrderLine& entity);
        void Insert(size_t batchId, const std::vector<Domain::OrderLine>& orders);
        void RemoveByBatchId(size_t batchId);
    private:
        Poco::Data::Session& _session;
    };
}