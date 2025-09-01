#pragma once

#include "Precompile.hpp"

#include "Domain/Product/OrderLine.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    class OrderLineMapper
    {
    public:
        explicit OrderLineMapper(Poco::Data::Session& session);

        [[nodiscard]] std::vector<Domain::OrderLine> FindByBatchId(int batchId);
        void Insert(int batchId, const std::vector<Domain::OrderLine>& orders);
        void RemoveByBatchesId(std::vector<int> batchesId);

    private:
        Poco::Data::Session& _session;
    };
}