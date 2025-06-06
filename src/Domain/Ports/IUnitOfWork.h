#pragma once

#include "Forwards.h"


namespace Allocation::Domain
{
    class IUnitOfWork
    {
    public:
        virtual ~IUnitOfWork() = default;

        virtual void Commit() = 0;
        virtual void RollBack() = 0;
        virtual bool IsCommited() const = 0;

        virtual IRepository& GetBatchRepository() = 0;
    };
   
}