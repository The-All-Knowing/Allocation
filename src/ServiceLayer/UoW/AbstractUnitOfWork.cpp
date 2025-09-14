#include "AbstractUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    AbstractUnitOfWork::AbstractUnitOfWork(Domain::IUpdatableRepository& repo) : _tracking(repo) {}

    void AbstractUnitOfWork::Commit()
    {
        try
        {
            for (const auto& [product, _] : _tracking.GetSeen())
                if (product->IsModified())
                    _tracking.Add(product);
            _isCommited = true;
        }
        catch (...)
        {
            _isCommited = false;
            throw;  // Re-throw the exception to preserve original error information
        }
    }

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