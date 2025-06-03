#pragma once

#include "Batch.h"


namespace Allocation::Domain
{
    class IRepository
    {
    public:
        virtual ~IRepository() = default;

        virtual void Add(const Domain::Batch& batch) = 0;

        virtual std::optional<Batch> Get(const std::string& reference) = 0;

        virtual std::vector<Batch> List() = 0;
    };
   
}