#include "SqlRepository.hpp"


namespace Allocation::Adapters::Repository
{
    SqlRepository::SqlRepository(const Poco::Data::Session& session) : _mapper(session) {}

    void SqlRepository::Add(Domain::ProductPtr product)
    {
        if(!product)
            throw std::invalid_argument("The nullptr product");
        _mapper.Insert(product);
    }

    Domain::ProductPtr SqlRepository::Get(const std::string& SKU)
    {
        return _mapper.FindBySKU(std::string(SKU));
    }

    Domain::ProductPtr SqlRepository::GetByBatchRef(const std::string& batchRef)
    {
        return _mapper.FindByBatchRef(std::string(batchRef));
    }

    void SqlRepository::Update(Domain::ProductPtr product, int oldVersion)
    {
        if(!product)
            throw std::invalid_argument("The nullptr product");
        if(!_mapper.Update(product, oldVersion));
            std::runtime_error("Could not serialize access due to concurrent update");
    }
}