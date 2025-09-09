#include "MessageBus.hpp"

#include "Domain/Events/OutOfStock.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::ServiceLayer
{
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
            if (message->GetType() == Domain::IMessage::Type::Command)
            {
                if (!_commandHandlers.contains(typeid(*message)))
                    throw std::runtime_error(
                        std::format("The {} command doesn`t have a handler", message->Name()));

                HandleCommand(uow,
                    std::static_pointer_cast<Domain::Commands::AbstractCommand>(message), queue);
            }
            else if (message->GetType() == Domain::IMessage::Type::Event &&
                     _eventHandlers.contains(typeid(*message)))
            {
                HandleEvent(
                    uow, std::static_pointer_cast<Domain::Events::AbstractEvent>(message), queue);
            }
        }
    }

    void MessageBus::Handle(Domain::IMessagePtr message)
    {
        ServiceLayer::UoW::SqlUnitOfWork uow;
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
            _commandHandlers.at(typeid(*command))(uow, command);
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