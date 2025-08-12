#pragma once

#include "IMessage.hpp"
#include "IRepository.hpp"


namespace Allocation::Domain
{
    class IUnitOfWork
    {
    public:
        virtual ~IUnitOfWork() = default;

        virtual void Commit() = 0;
        virtual void RollBack() = 0;
        [[nodiscard]] virtual bool IsCommited() const noexcept = 0;
        [[nodiscard]] virtual IRepository& GetProductRepository() = 0;
        [[nodiscard]] virtual std::vector<IMessagePtr> GetNewMessages() noexcept = 0;
    };
}