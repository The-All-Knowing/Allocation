#pragma once

#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services::UoW
{
    
    class AbstractUnitOfWork : public Domain::IUnitOfWork
    {
    public:
        void Commit() override;
        void RollBack() override;
        bool IsCommited() const noexcept override;

        void PublishEvents(const std::vector<std::shared_ptr<Domain::Product>>& products);

    private:
        bool _isCommited{false};
    };
}