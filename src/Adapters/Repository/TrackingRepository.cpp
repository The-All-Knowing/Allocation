#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo){};

    void TrackingRepository::Add(const Domain::Product& product)
    {
        _repo.Add(product);
        if (!_seen.contains(product.GetSKU()))
            _seenObjByOldVersion[product.GetSKU()] = product.GetVersion();
        _seen[product.GetSKU()] = std::make_shared<Domain::Product>(product);
    }

    Domain::ProductPtr TrackingRepository::Get(std::string_view SKU)
    {
        auto product = _repo.Get(SKU);
        if (product)
        {
            _seen[product->GetSKU()] = product;
            _seenObjByOldVersion[product->GetSKU()] = product->GetVersion();
        }

        return product;
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(std::string_view batchRef)
    {
        auto product = _repo.GetByBatchRef(batchRef);
        if (product)
        {
            _seen[product->GetSKU()] = product;
            _seenObjByOldVersion[product->GetSKU()] = product->GetVersion();
        }

        return product;
    }

    std::vector<Domain::ProductPtr> TrackingRepository::GetSeen() const noexcept
    {
        std::vector<Domain::ProductPtr> result;
        result.reserve(_seen.size());
        for (const auto& [_, product] : _seen)
            result.push_back(product);

        return result;
    }

    std::vector<std::tuple<std::string, size_t, size_t>> TrackingRepository::GetChangedVersions()
        const noexcept
    {
        std::vector<std::tuple<std::string, size_t, size_t>> result;

        for (auto& [sku, prod] : _seen)
        {
            auto oldVersionIt = _seenObjByOldVersion.find(sku);
            if (oldVersionIt == _seenObjByOldVersion.end())
                continue;

            size_t oldVersion = oldVersionIt->second;
            if (prod->GetVersion() == oldVersion)
                continue;

            result.emplace_back(sku, oldVersion, prod->GetVersion());
        }

        return result;
    }
}