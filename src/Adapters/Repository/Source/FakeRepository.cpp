#pragma once

#include "FakeRepository.h"


namespace Allocation::Adapters::Repository
{
    
    void FakeRepository::Add(const Domain::Batch& batch)
    {
        _batches.insert_or_assign(batch.GetReference(), batch);
    }

    std::optional<Domain::Batch> FakeRepository::Get(const std::string& reference)
    {
        auto it = _batches.find(reference);
        if (it != _batches.end())
            return it->second;
        return std::nullopt;
    }

    std::vector<Domain::Batch> FakeRepository::List()
    {
        std::vector<Domain::Batch> result;
        for (const auto& [_, batch] : _batches)
            result.push_back(batch);
        return result;
    }

}