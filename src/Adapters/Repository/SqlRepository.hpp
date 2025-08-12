#pragma once

#include "Domain/Ports/IRepository.hpp"
#include "Precompile.hpp"


namespace Allocation::Adapters::Repository
{
    class SqlRepository final : public Domain::IRepository
    {
    public:
        SqlRepository(Poco::Data::Session& session);
        void Add(std::shared_ptr<Domain::Product> product) override;
        [[nodiscard]] std::shared_ptr<Domain::Product> Get(std::string_view SKU) override;
        [[nodiscard]] std::shared_ptr<Domain::Product> GetByBatchRef(std::string_view ref) override;

        void UpdateVersion(std::string_view SKU, size_t old, size_t newVersion);

    private:
        Poco::Data::Session& _session;
    };
}