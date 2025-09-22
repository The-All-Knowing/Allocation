#include "AbstractUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    AbstractUnitOfWork::AbstractUnitOfWork(Domain::IUpdatableRepository& repo) : _tracking(repo) {}

    void AbstractUnitOfWork::Commit()
    {
        for (const auto& [product, _] : _tracking.GetSeen())
            if (product->IsModified())
                _tracking.Add(product);
        _isCommitted = true;
    }

    void AbstractUnitOfWork::RollBack() { _isCommitted = false; }

    bool AbstractUnitOfWork::IsCommitted() const noexcept { return _isCommitted; }

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