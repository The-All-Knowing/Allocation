#include "FakeRepository_test.hpp"


namespace Allocation::Tests
{
    FakeRepository::FakeRepository(const std::vector<Domain::ProductPtr>& init)
    {
        for (const auto& prod : init)
            _skuByProduct.insert({prod->GetSKU(), prod});
    }

    void FakeRepository::Add(Domain::ProductPtr product)
    {
        _skuByProduct.insert_or_assign(
            product->GetSKU(), product);
    }

    Domain::ProductPtr FakeRepository::Get(std::string_view SKU)
    {
        auto it = _skuByProduct.find(std::string(SKU));
        if (it != _skuByProduct.end())
            return it->second;
        return nullptr;
    }

    Domain::ProductPtr FakeRepository::GetByBatchRef(std::string_view batchRef)
    {
        for (const auto& [_, product] : _skuByProduct)
        {
            const auto& batches = product->GetBatches();
            if (std::any_of(batches.begin(), batches.end(),
                    [batchRef](const auto& batch) { return batch.GetReference() == batchRef; }))
            {
                return product;
            }
        }
        return nullptr;
    }
}