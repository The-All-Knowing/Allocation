#include "TrackingRepository.h"


namespace Allocation::Adapters::Repository
{

    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo)
    {};

    void TrackingRepository::Add(std::shared_ptr<Domain::Product> product)
    {
        _repo.Add(product);
        if (!_seen.contains(product->GetSKU()))
            _seenObjByOldVersion[product->GetSKU()] = product->GetVersion();
        _seen[product->GetSKU()] = product;
    }
    
    std::shared_ptr<Domain::Product> TrackingRepository::Get(std::string_view SKU)
    {
        auto product = _repo.Get(SKU);
        if (product)
        {
            _seen[product->GetSKU()] = product;
            _seenObjByOldVersion[product->GetSKU()] = product->GetVersion(); 
        }

        return product;
    }

    std::vector<std::shared_ptr<Domain::Product>> TrackingRepository::GetSeen() const noexcept
    {
        std::vector<std::shared_ptr<Domain::Product>> result;
        result.reserve(_seen.size());
        for (const auto& [_, product] : _seen)
            result.push_back(product);

        return result;
    }

    std::vector<std::tuple<std::string, size_t, size_t>> TrackingRepository::GetChangedVersions() const noexcept
    {
        std::vector<std::tuple<std::string, size_t, size_t>> result;

        for(auto& [sku, prod] : _seen)
        {
            auto oldVersionIt = _seenObjByOldVersion.find(sku);
            if(oldVersionIt == _seenObjByOldVersion.end())
                continue;

            size_t oldVersion = oldVersionIt->second;
            if(prod->GetVersion() == oldVersion)
                continue;
            
            result.emplace_back(sku, oldVersion, prod->GetVersion());
        }

        return result;
    }
}