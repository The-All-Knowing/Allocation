#pragma once

#include "Domain/Product/Batch.hpp"
#include "Precompile.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    class BatchMapper
    {
    public:
        explicit BatchMapper(Poco::Data::Session& session);

        [[nodiscard]] std::vector<Domain::Batch> GetBySKU(const std::string& SKU);
        void Update(const std::vector<Domain::Batch>& batches, const std::string& SKU);
        void Insert(const std::vector<Domain::Batch>& batches);

    private:
        void DeleteBatches(std::string SKU);
        [[nodiscard]] int GetIdBatch(std::string reference);
        [[nodiscard]] std::vector<int> GetIdBatches(std::string SKU);

        Poco::Data::Session& _session;
    };
}