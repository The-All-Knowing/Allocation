#pragma once

#include "Precompile.h"
#include "Domain/Include/Ports/IRepository.h"


namespace Allocation::Adapters::Repository
{

    class FakeRepository : public Domain::IRepository
    {
    public:
        FakeRepository(const std::vector<Domain::Batch>& init);

        void Add(const Domain::Batch& batch);

        std::optional<Domain::Batch> Get(const std::string& reference);

        std::vector<Domain::Batch> List();
    private:
        std::unordered_map<std::string, Domain::Batch> _batches;
    };

}