#pragma once

#include "FakeRepository.h"


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
}