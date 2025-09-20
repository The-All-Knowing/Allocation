#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IUpdatableRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("The nullptr product");

        auto sku = product->GetSKU();
        if (auto it = _skuToProductAndOldVersion.find(sku); it != _skuToProductAndOldVersion.end())
            Update(product, it->second.second);
        else
        {
            _repo.Add(product);
            product->SetModified(false);
            _skuToProductAndOldVersion.insert({sku, {product, product->GetVersion()}});
        }
    }

    Domain::ProductPtr TrackingRepository::Get(const std::string& sku)
    {
        if (sku.empty())
            return nullptr;

        if (auto it = _skuToProductAndOldVersion.find(sku); it != _skuToProductAndOldVersion.end())
            return it->second.first;

        auto product = _repo.Get(sku);
        if (!product)
            return nullptr;
        _skuToProductAndOldVersion.insert({product->GetSKU(), {product, product->GetVersion()}});
        return product;
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(const std::string& batchRef)
    {
        if (batchRef.empty())
            return nullptr;

        auto it = std::find_if(_skuToProductAndOldVersion.begin(), _skuToProductAndOldVersion.end(),
            [batchRef](auto& pair)
            { return pair.second.first->GetBatch(batchRef) != std::nullopt; });

        if (it != _skuToProductAndOldVersion.end())
            return it->second.first;

        auto product = _repo.GetByBatchRef(batchRef);
        if (!product)
            return nullptr;

        _skuToProductAndOldVersion.insert({product->GetSKU(), {product, product->GetVersion()}});
        return product;
    }

    std::vector<std::pair<Domain::ProductPtr, int>> TrackingRepository::GetSeen() const noexcept
    {
        std::vector<std::pair<Domain::ProductPtr, int>> result;
        result.reserve(_skuToProductAndOldVersion.size());
        for (const auto& [_, productAndOldVersion] : _skuToProductAndOldVersion)
            result.push_back(productAndOldVersion);

        return result;
    }

    void TrackingRepository::Clear() noexcept { _skuToProductAndOldVersion.clear(); }

    void TrackingRepository::Update(Domain::ProductPtr product, int oldVersion)
    {
        if (!product)
            throw std::invalid_argument("The nullptr product");

        _repo.Update(product, oldVersion);
        product->SetModified(false);
    }
}
