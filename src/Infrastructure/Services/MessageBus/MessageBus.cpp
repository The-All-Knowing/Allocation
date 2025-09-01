#include "MessageBus.hpp"

#include "Domain/Events/OutOfStock.hpp"
#include "Handlers.hpp"
#include "Infrastructure/Services/Loggers/ILogger.hpp"
#include "Infrastructure/Services/UoW/SqlUnitOfWork.hpp"


namespace Allocation::Services
{
    MessageBus::MessageBus()
    {
        SubscribeToEvent<Allocation::Domain::Events::Allocated>(Handlers::PublishAllocatedEvent);
        SubscribeToEvent<Allocation::Domain::Events::Allocated>(Handlers::AddAllocationToReadModel);
        SubscribeToEvent<Allocation::Domain::Events::Deallocated>(
            Handlers::RemoveAllocationFromReadModel);
        SubscribeToEvent<Allocation::Domain::Events::Deallocated>(Handlers::Reallocate);
        SubscribeToEvent<Allocation::Domain::Events::OutOfStock>(
            Handlers::SendOutOfStockNotification);

        SetCommandHandler<Allocation::Domain::Commands::Allocate>(Handlers::Allocate);
        SetCommandHandler<Allocation::Domain::Commands::CreateBatch>(Handlers::AddBatch);
        SetCommandHandler<Allocation::Domain::Commands::ChangeBatchQuantity>(
            Handlers::ChangeBatchQuantity);
    }

    MessageBus& MessageBus::Instance()
    {
        static MessageBus messageBus;
        return messageBus;
    }

    void MessageBus::Handle(Domain::IMessagePtr message, Domain::IUnitOfWork& uow)
    {
        std::vector<std::string> result;
        std::queue<Domain::IMessagePtr> queue;
        queue.push(message);

        while (!queue.empty())
        {
            auto message = queue.front();
            queue.pop();
            if (message->GetType() == Domain::IMessage::Type::Event &&
                _eventHandlers.contains(typeid(*message)))
            {
                HandleEvent(
                    uow, std::static_pointer_cast<Domain::Events::AbstractEvent>(message), queue);
            }
            else if (message->GetType() == Domain::IMessage::Type::Command &&
                     _commandHandlers.contains(typeid(*message)))
            {
                HandleCommand(uow,
                    std::static_pointer_cast<Domain::Commands::AbstractCommand>(message), queue);
            }
            else
            {
                throw std::runtime_error(
                    std::format("{} was not an Event or Command", message->Name()));
            }
        }
    }

    void MessageBus::Handle(Domain::IMessagePtr message)
    {
        Services::UoW::SqlUnitOfWork uow;
        Handle(message, uow);
    }

    void MessageBus::HandleEvent(Domain::IUnitOfWork& uow, Domain::Events::EventPtr event,
        std::queue<Domain::IMessagePtr>& queue) noexcept
    {
        for (auto& handler : _eventHandlers[typeid(*event)])
        {
            try
            {
                Loggers::GetLogger()->Debug(std::format("Handling event {} with handler {}",
                    event->Name(), handler.target_type().name()));
                handler(uow, event);
                for (auto& newMessage : uow.GetNewMessages())
                    queue.push(newMessage);
            }
            catch (...)
            {
                Loggers::GetLogger()->Error(
                    std::format("Exception handling event {}", event->Name()));
            }
        }
    }

    void MessageBus::HandleCommand(Domain::IUnitOfWork& uow, Domain::Commands::CommandPtr command,
        std::queue<Domain::IMessagePtr>& queue)
    {
        Loggers::GetLogger()->Debug(std::format("handling command {}", command->Name()));
        try
        {
            _commandHandlers[typeid(*command)](uow, command);
            for (auto& newMessage : uow.GetNewMessages())
                queue.push(newMessage);
        }
        catch (...)
        {
            Loggers::GetLogger()->Error(
                std::format("Exception handling command {}", command->Name()));
            throw;
        }
    }
}