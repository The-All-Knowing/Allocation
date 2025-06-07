#pragma once

#include "Precompile.h"
#include "AbstractUnitOfWork.h"
#include "Adapters/Repository/FakeRepository.h"


namespace Allocation::Services::UoW
{
    class FakeUnitOfWork final : public AbstractUnitOfWork 
    {
    public:
        FakeUnitOfWork() : _repo(std::make_shared<Adapters::Repository::FakeRepository>()) {}
        FakeUnitOfWork(std::shared_ptr<Adapters::Repository::FakeRepository> repo) : _repo(repo) {}

        Domain::IRepository& GetProductRepository()
        {
            return *_repo;
        }

    private:
        std::shared_ptr<Adapters::Repository::FakeRepository> _repo;
    };
}