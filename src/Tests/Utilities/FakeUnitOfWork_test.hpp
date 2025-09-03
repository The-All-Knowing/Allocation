#pragma once

#include "Precompile.hpp"

#include "Adapters/Repository/TrackingRepository.hpp"
#include "FakeRepository_test.hpp"
#include "Services/UoW/AbstractUnitOfWork.hpp"


namespace Allocation::Tests
{
    class FakeUnitOfWork final : public Services::UoW::AbstractUnitOfWork
    {
    public:
        FakeUnitOfWork() : _tracking(_repo) {}

        std::optional<Poco::Data::Session> GetSession() noexcept { return std::nullopt; }

        [[nodiscard]] Domain::IRepository& GetProductRepository() { return _tracking; }

        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override
        {
            std::vector<Domain::IMessagePtr> result;

            for (const auto& product : _tracking.GetSeen())
            {
                auto& messages = product->Messages();
                result.insert(result.end(), messages.begin(), messages.end());
                product->ClearMessages();
            }

            return result;
        }

    private:
        FakeRepository _repo;
        Adapters::Repository::TrackingRepository _tracking;
    };
}