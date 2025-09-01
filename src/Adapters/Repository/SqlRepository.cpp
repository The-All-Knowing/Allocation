#include "SqlRepository.hpp"

#include "Adapters/Database/Mappers/ProductMapper.hpp"


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(Poco::Data::Session session) : _session(session)
    {
    }

    void SqlRepository::Add(const Domain::Product& product)
    {
        Database::Mapper::ProductMapper mapper(_session);

        if (mapper.IsExists(product.GetSKU()))
            mapper.Update(product);
        else
            mapper.Insert(product);
    }

    Domain::ProductPtr SqlRepository::Get(std::string_view SKU)
    {
        Database::Mapper::ProductMapper mapper(_session);
        return mapper.FindBySKU(std::string(SKU));
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(std::string_view batchRef)
    {
        Database::Mapper::ProductMapper mapper(_session);
        return mapper.FindByBatchRef(std::string(batchRef));
    }

    void SqlRepository::UpdateVersion(std::string_view SKU, size_t old, size_t newVersion)
    {
        Database::Mapper::ProductMapper mapper(_session);
        if (!mapper.UpdateVersion(std::string(SKU), old, newVersion))
            throw std::runtime_error("Could not serialize access due to concurrent update");
    }
}