#pragma once

#include "IRepository.h"
#include "IMessage.h"


namespace Allocation::Domain
{
    
    class IUnitOfWork
    {
    public:
        virtual ~IUnitOfWork() = default;

        virtual void Commit() = 0;
        virtual void RollBack() = 0;
        virtual bool IsCommited() const noexcept = 0;
        virtual IRepository& GetProductRepository() = 0;
        virtual std::vector<IMessagePtr> GetNewMessages() noexcept = 0;
    };
}