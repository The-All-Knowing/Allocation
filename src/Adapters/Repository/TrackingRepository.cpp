#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        _repo.Add(product);
        if (!_seen.contains(product->GetSKU()))
        {
            _seen.insert_or_assign(product->GetSKU(), product);
            _seenObjByOldVersion.insert_or_assign(product->GetSKU(), product->GetVersion());
        }
    }

    Domain::ProductPtr TrackingRepository::Get(std::string_view SKU)
    {
        auto product = _repo.Get(SKU);
        if (product)
        {
            _seen.insert_or_assign(product->GetSKU(), product);
            _seenObjByOldVersion.insert_or_assign(product->GetSKU(), product->GetVersion());
        }
        return product;
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(std::string_view batchRef)
    {
        auto product = _repo.GetByBatchRef(batchRef);
        if (product)
        {
            _seen.insert_or_assign(product->GetSKU(), product);
            _seenObjByOldVersion.insert_or_assign(product->GetSKU(), product->GetVersion());
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

    std::vector<std::tuple<std::string, size_t, size_t>> TrackingRepository::GetChangedVersions() const noexcept
    {
        std::vector<std::tuple<std::string, size_t, size_t>> result;

        for (const auto& [sku, product] : _seen)
        {
            size_t oldVersion = _seenObjByOldVersion.at(sku);
            size_t newVersion = product->GetVersion();
            if (newVersion == oldVersion)
                continue;

            result.emplace_back(sku, oldVersion, newVersion);
        }

        return result;
    }
}
