#pragma once

#include "Precompile.h"
#include "Batch.h"


namespace Allocation::Adapters::Database::Mapper
{

    class BatchMapper
    {
    public:
        BatchMapper(Poco::Data::Session& session);

        std::optional<int> GetBatchId(std::string reference);

        std::vector<Domain::Batch> GetAll();

        void Insert(const Domain::Batch& batch);

        void Update(int id, const Domain::Batch& batch);

        std::optional<Domain::Batch> FindByReference(std::string reference);
    private:
        void UpdateAllocations(size_t batchId, const std::vector<Domain::OrderLine>& orders);

    private:
        Poco::Data::Session& _session;
    };

}