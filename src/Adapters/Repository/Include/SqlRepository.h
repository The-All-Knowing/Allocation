#pragma once

#include "Precompile.h"
#include "Domain/Include/Ports/IRepository.h"


namespace Allocation::Adapters::Repository
{

    class SqlRepository : public Domain::IRepository
    {
    public:
        SqlRepository(Poco::Data::Session& session);

        void Add(const Domain::Batch& batch);

        std::optional<Domain::Batch> Get(const std::string& reference);

        std::vector<Domain::Batch> List();
    private:
        Poco::Data::Session& _session;
    };

}