#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IUpdatableRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        if (!product)
            throw std::invalid_argument("The nullptr product");

        if (auto it = _seenAndOldVersion.find(product->GetSKU()); it != _seenAndOldVersion.end())
            Update(product, it->second.second);
        else
        {
            _repo.Add(product);
            _seenAndOldVersion.insert({product->GetSKU(), {product, product->GetVersion()}});
        }
    }

    Domain::ProductPtr TrackingRepository::Get(const std::string& SKU)
    {
        if (auto it = _seenAndOldVersion.find(SKU); it != _seenAndOldVersion.end())
            return it->second.first;

        auto product = _repo.Get(SKU);
        if (!product)
            return nullptr;
        _seenAndOldVersion.insert({product->GetSKU(), {product, product->GetVersion()}});
        return product;
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(const std::string& batchRef)
    {
        auto it = std::find_if(_seenAndOldVersion.begin(), _seenAndOldVersion.end(),
            [batchRef](auto& pair)
            { return pair.second.first->GetBatch(batchRef) != std::nullopt; });

        if (it != _seenAndOldVersion.end())
            return it->second.first;

        auto product = _repo.GetByBatchRef(batchRef);
        if (!product)
            return nullptr;

        _seenAndOldVersion.insert({product->GetSKU(), {product, product->GetVersion()}});
        return product;
    }

    std::vector<std::pair<Domain::ProductPtr, int>> TrackingRepository::GetSeen() const noexcept
    {
        std::vector<std::pair<Domain::ProductPtr, int>> result;
        result.reserve(_seenAndOldVersion.size());
        for (const auto& [_, productAndOldVersion] : _seenAndOldVersion)
            result.push_back(productAndOldVersion);

        return result;
    }

    void TrackingRepository::Clear() noexcept { _seenAndOldVersion.clear(); }

    void TrackingRepository::Update(Domain::ProductPtr product, int oldVersion)
    {
        _repo.Update(product, oldVersion);
    }
}
