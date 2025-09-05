#include "AbstractUnitOfWork.hpp"


namespace Allocation::Services::UoW
{
    AbstractUnitOfWork::AbstractUnitOfWork(Domain::IRepository& repo) : _tracking(repo) {}

    void AbstractUnitOfWork::Commit() { _isCommited = true; }

    void AbstractUnitOfWork::RollBack() { _isCommited = false; }

    bool AbstractUnitOfWork::IsCommited() const noexcept { return _isCommited; }

    Domain::IRepository& AbstractUnitOfWork::GetProductRepository() { return _tracking; }

    std::vector<Domain::IMessagePtr> AbstractUnitOfWork::GetNewMessages() noexcept
    {
        std::vector<Domain::IMessagePtr> newMessages;
        for (const auto& [product, _] : _tracking.GetSeen())
        {
            auto messages = product->Messages();
            newMessages.insert(newMessages.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }

        return newMessages;
    }
}