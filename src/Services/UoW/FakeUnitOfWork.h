#pragma once

#include "Precompile.h"
#include "AbstractUnitOfWork.h"
#include "Adapters/Repository/FakeRepository.h"
#include "Adapters/Repository/TrackingRepository.h"


namespace Allocation::Services::UoW
{
    class FakeUnitOfWork final : public AbstractUnitOfWork 
    {
    public:
        FakeUnitOfWork() : _repo(std::make_shared<Adapters::Repository::FakeRepository>()), _tracking(*_repo)
        {}

        FakeUnitOfWork(std::shared_ptr<Adapters::Repository::FakeRepository> repo) : _repo(repo), _tracking(*_repo)
        {}

        Domain::IRepository& GetProductRepository()
        {
            return _tracking;
        }

        void Commit() override
        {
            AbstractUnitOfWork::Commit();
            AbstractUnitOfWork::PublishEvents(_tracking.GetSeen());
        }

    private:
        std::shared_ptr<Adapters::Repository::FakeRepository> _repo;
        Adapters::Repository::TrackingRepository _tracking;
    };
}