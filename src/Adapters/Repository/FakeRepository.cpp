#include "FakeRepository.hpp"


namespace Allocation::Adapters::Repository
{
    FakeRepository::FakeRepository(const std::vector<std::shared_ptr<Domain::Product>>& init)
    {
        for (const auto& prod : init)
            _skuByProduct.insert({prod->GetSKU(), prod});
    }

    void FakeRepository::Add(std::shared_ptr<Domain::Product> product)
    {
        _skuByProduct.insert_or_assign(product->GetSKU(), product);
    }

    std::shared_ptr<Domain::Product> FakeRepository::Get(std::string_view SKU)
    {
        auto it = _skuByProduct.find(std::string(SKU));
        if (it != _skuByProduct.end())
            return it->second;
        return nullptr;
    }

    std::shared_ptr<Domain::Product> FakeRepository::GetByBatchRef(std::string_view ref)
    {
        for (const auto& [_, product] : _skuByProduct)
        {
            const auto& batches = product->GetBatches();
            if (std::any_of(batches.begin(), batches.end(),
                    [ref](const auto& batch) { return batch.GetReference() == ref; }))
            {
                return product;
            }
        }
        return nullptr;
    }
}