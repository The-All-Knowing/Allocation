#include "MessageBus.h"
#include "Loggers/PocoLogger.h"
#include "Domain/Events/OutOfStock.h"
#include "Handlers.h"


namespace Allocation::Services
{
    MessageBus::MessageBus() : _logger(std::make_shared<Loggers::PocoLogger>())
    {
        SubscribeToEvent<Allocation::Domain::Events::OutOfStock>(Handlers::SendOutOfStockNotification);

        SetCommandHandler<Allocation::Domain::Commands::Allocate>(Handlers::Allocate);
        SetCommandHandler<Allocation::Domain::Commands::CreateBatch>(Handlers::AddBatch);
        SetCommandHandler<Allocation::Domain::Commands::ChangeBatchQuantity>(Handlers::ChangeBatchQuantity);
    }

    MessageBus& MessageBus::Instance()
    {
        static MessageBus messageBus;
        return messageBus;
    }

    void MessageBus::SetLogger(Loggers::ILoggerPtr logger)
    {
        _logger = logger;
    }

    std::vector<std::string> MessageBus::Handle(UoWFactory uowFactory, const CommandPtr& command)
    {
        std::vector<std::string> result;
        std::queue<Domain::IMessagePtr> queue;
        queue.push(command);

        while (!queue.empty())
        {
            auto uow = uowFactory();
            auto message = queue.front();
            queue.pop();
            if (message->GetType() == Domain::IMessage::Type::Event &&
                _eventHandlers.contains(typeid(*message)))
            {
                HandleEvent(uow, std::static_pointer_cast<Domain::Events::AbstractEvent>(message), queue);
            }
            else if (message->GetType() == Domain::IMessage::Type::Command &&
                    _commandHandlers.contains(typeid(*message)))
            {
                auto res = HandleCommand(uow, std::static_pointer_cast<Domain::Commands::AbstractCommand>(message), queue);
                if (res.has_value())
                    result.push_back(res.value());
            }
            else
            {
                throw std::runtime_error(std::format("{} was not an Event or Command", message->Name()));
            }
        }

        return result;
    }

    void MessageBus::HandleEvent(UoWPtr uow, EventPtr event, std::queue<Domain::IMessagePtr>& queue) noexcept
    {
        for (auto& handler : _eventHandlers[typeid(*event)])
        {
            try
            {
                _logger->Debug(std::format("Handling event {} with handler {}", event->Name(), handler.target_type().name()));
                handler(uow, event);
                for (auto& newMessage : uow->GetNewMessages())
                    queue.push(newMessage);
            }
            catch(...)
            {
                _logger->Error(std::format("Exception handling event {}", event->Name()));
            }
            
        }
    }

    std::optional<std::string> MessageBus::HandleCommand(UoWPtr uow, CommandPtr command, std::queue<Domain::IMessagePtr>& queue)
    {
        _logger->Debug(std::format("handling command {}", command->Name()));
        try
        {
            auto result = _commandHandlers[typeid(*command)](uow, command);
            for (auto& newMessage : uow->GetNewMessages())
                queue.push(newMessage);
            return result;
        }
        catch(...)
        {
            _logger->Error(std::format("Exception handling command {}", command->Name()));
            throw;
        }
    }
}