#pragma once

#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::Services::UoW
{
    class AbstractUnitOfWork : public Domain::IUnitOfWork
    {
    public:
        void Commit() override;
        void RollBack() override;
        bool IsCommited() const noexcept override;

    private:
        bool _isCommited{false};
    };
}