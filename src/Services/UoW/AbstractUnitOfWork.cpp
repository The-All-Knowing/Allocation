#include "AbstractUnitOfWork.h"
#include "EventBus/EventBus.h"


namespace Allocation::Services::UoW
{
    void AbstractUnitOfWork::Commit()
    {
        _isCommited = true;
    }

    void AbstractUnitOfWork::RollBack()
    {
        _isCommited = false;
    }

    bool AbstractUnitOfWork::IsCommited() const noexcept
    {
        return _isCommited;
    }

    void AbstractUnitOfWork::PublishEvents(const std::vector<std::shared_ptr<Domain::Product>>& products)
    {
        auto& messagebus = Allocation::Services::EventBus::Instance();

        for(const auto& product : products)
            for (auto& event : product->Events())
                messagebus.Publish(event);
    }   
}