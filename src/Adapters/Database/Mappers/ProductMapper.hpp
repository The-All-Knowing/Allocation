#pragma once

#include "Domain/Product/Product.hpp"
#include "Precompile.hpp"


namespace Allocation::Adapters::Database::Mapper
{
    class ProductMapper
    {
    public:
        explicit ProductMapper(Poco::Data::Session& session);

        [[nodiscard]] bool IsExists(std::string SKU);
        [[nodiscard]] std::shared_ptr<Domain::Product> FindBySKU(std::string SKU);
        [[nodiscard]] std::shared_ptr<Domain::Product> FindByBatchRef(std::string ref);
        void Update(std::shared_ptr<Domain::Product> product);
        void Insert(std::shared_ptr<Domain::Product> product);
        [[nodiscard]] bool UpdateVersion(std::string SKU, size_t oldVersion, size_t newVersion);

    private:
        Poco::Data::Session& _session;
    };
}