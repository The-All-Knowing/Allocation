#pragma once

#include "Precompile.h"
#include "OrderLine.h"


namespace Allocation::Adapters::Database::Mapper
{
    class OrderLineMapper
    {
    public:
        OrderLineMapper(Poco::Data::Session& session);
        std::optional<Domain::OrderLine> FindById(size_t id);
        std::vector<Domain::OrderLine> FindAll();
        void Insert(const Domain::OrderLine& entity);
        void Update(const Domain::OrderLine& entity);
        void Remove(size_t id);
    private:
        Poco::Data::Session& _session;
    };
}