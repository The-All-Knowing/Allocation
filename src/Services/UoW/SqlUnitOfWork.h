#pragma once

#include "Precompile.h"
#include "Forwards.h"
#include "AbstractUnitOfWork.h"
#include "Domain/Ports/IRepository.h"
#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services::UoW
{
    class SqlUnitOfWork : public AbstractUnitOfWork 
    {
        struct Impl;

    public:
        SqlUnitOfWork();
        ~SqlUnitOfWork();

        void Commit() override;
        void RollBack() override;

        Domain::IRepository& GetBatchRepository() override;

    private:
        std::unique_ptr<Impl> _impl;
    };
}