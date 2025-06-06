#pragma once

#include "Precompile.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Adapters::Repository
{

    class SqlRepository : public Domain::IRepository
    {
    public:
        SqlRepository(Poco::Data::Session& session);

        SqlRepository(const SqlRepository&) = delete;
        SqlRepository(SqlRepository&&) = delete;

        SqlRepository& operator=(const SqlRepository&) = delete;
        SqlRepository& operator=(SqlRepository&&) = delete;

        void Add(const Domain::Batch& batch);

        std::optional<Domain::Batch> Get(const std::string& reference);

        std::vector<Domain::Batch> List();
    private:
        Poco::Data::Session& _session;
    };

}