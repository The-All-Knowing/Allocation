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

        std::vector<Domain::Events::IEventPtr> GetNewEvents() noexcept override
        {
            std::vector<Domain::Events::IEventPtr> result;

            for (const auto& product : _tracking.GetSeen())
            {
                auto& events = product->Events();
                result.insert(result.end(), events.begin(), events.end());
                product->ClearEvents();
            }

            return result;
        }

    private:
        std::shared_ptr<Adapters::Repository::FakeRepository> _repo;
        Adapters::Repository::TrackingRepository _tracking;
    };
}