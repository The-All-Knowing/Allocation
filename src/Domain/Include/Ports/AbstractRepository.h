#pragma once

#include "Precompile.h"
#include "Batch.h"


namespace Allocation::Domain
{
    class AbstractRepository
    {
    public:
        virtual ~AbstractRepository() = default;

        virtual void Add(Batch batch) = 0;

        virtual Batch Get(const std::string& reference) = 0;

        virtual std::vector<Batch> List() = 0;
    };
   
}