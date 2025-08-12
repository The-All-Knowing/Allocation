#include "SqlRepository.hpp"

#include "Adapters/Database/Mappers/ProductMapper.hpp"


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(Poco::Data::Session& session) : _session(session) {}

    void SqlRepository::Add(std::shared_ptr<Domain::Product> product)
    {
        Database::Mapper::ProductMapper mapper(_session);

        if (mapper.IsExists(product->GetSKU()))
            mapper.Update(product);
        else
            mapper.Insert(product);
    }

    std::shared_ptr<Domain::Product> SqlRepository::Get(std::string_view SKU)
    {
        Database::Mapper::ProductMapper mapper(_session);
        return mapper.FindBySKU(std::string(SKU));
    }

    std::shared_ptr<Domain::Product> SqlRepository::GetByBatchRef(std::string_view ref)
    {
        Database::Mapper::ProductMapper mapper(_session);
        return mapper.FindByBatchRef(std::string(ref));
    }

    void SqlRepository::UpdateVersion(std::string_view SKU, size_t old, size_t newVersion)
    {
        Database::Mapper::ProductMapper mapper(_session);
        if (!mapper.UpdateVersion(std::string(SKU), old, newVersion))
            throw std::runtime_error("Could not serialize access due to concurrent update");
    }
}