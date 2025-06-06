#pragma once

#include "Precompile.h"
#include "Forwards.h"
#include "AbstractUnitOfWork.h"
#include "Adapters/Repository/FakeRepository.h"


namespace Allocation::Services::UoW
{
    class FakeUnitOfWork : public AbstractUnitOfWork
    {
    public:
        FakeUnitOfWork(Adapters::Repository::FakeRepository& repo);

        Domain::IRepository& GetBatchRepository() override;

    private:
        Adapters::Repository::FakeRepository& _repo;
    };
}