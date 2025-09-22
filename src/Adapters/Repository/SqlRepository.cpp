#include "SqlRepository.hpp"


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(const Poco::Data::Session& session) : _mapper(session) {}

    void SqlRepository::Add(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("The nullptr product");
        _mapper.Insert(product);
    }

    Domain::ProductPtr SqlRepository::Get(const std::string& sku)
    {
        if (sku.empty())
            return nullptr;
        return _mapper.FindBySKU(sku);
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(const std::string& batchRef)
    {
        if (batchRef.empty())
            return nullptr;
        return _mapper.FindByBatchRef(batchRef);
    }

    void SqlRepository::Update(Domain::ProductPtr product, size_t oldVersion)
    {
        if (!product)
            throw std::invalid_argument("The nullptr product");
        if (!_mapper.Update(product, oldVersion))
            throw std::runtime_error("Could not serialize access due to concurrent update");
    }
}