#pragma once

#include "Precompile.h"
#include "Batch.h"
#include "OutOfStock.h"


namespace Allocation::Domain
{
    
    template<std::forward_iterator ForwardIt>
    Batch Allocate(const OrderLine& line, ForwardIt begin, ForwardIt end)
    {
        std::vector<std::reference_wrapper<Batch>> batches;
        batches.reserve(std::distance(begin, end));
        for (auto it = begin; it != end; ++it)
            batches.emplace_back(*it);

        std::sort(batches.begin(), batches.end(), 
            [](const Batch& lhs, const Batch& rhs) { return lhs < rhs; });

        for (const auto& batch_ref : batches)
        {
            Batch& batch = batch_ref.get();
            if (batch.CanAllocate(line))
            {
                batch.Allocate(line);
                return batch;
            }
        }

        throw OutOfStock(line.GetSKU());
    }

}