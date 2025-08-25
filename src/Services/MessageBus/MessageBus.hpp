#pragma once

#include "Domain/Commands/AbstractCommand.hpp"
#include "Domain/Events/AbstractEvent.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"
#include "Precompile.hpp"


namespace Allocation::Services
{
    class MessageBus
    {
        using UoWPtr = std::shared_ptr<Domain::IUnitOfWork>;
        using EventPtr = std::shared_ptr<Domain::Events::AbstractEvent>;
        using CommandPtr = std::shared_ptr<Domain::Commands::AbstractCommand>;

        using EventHandler = std::function<void(UoWPtr, EventPtr)>;
        using CommandHandler = std::function<std::optional<std::string>(UoWPtr, CommandPtr)>;
        using UoWFactory = std::function<UoWPtr()>;

    public:
        static MessageBus& Instance();

        template <typename T>
            requires std::derived_from<T, Domain::Events::AbstractEvent>
        void SubscribeToEvent(auto&& handler) noexcept
        {
            auto& handlers = _eventHandlers[typeid(T)];
            handlers.emplace_back(
                [h = std::forward<decltype(handler)>(handler)](UoWPtr uow, EventPtr event)
                { h(uow, std::static_pointer_cast<T>(event)); });
        }

        template <typename T>
            requires std::derived_from<T, Domain::Commands::AbstractCommand>
        void SetCommandHandler(auto&& handler) noexcept
        {
            _commandHandlers[typeid(T)] = [h = std::forward<decltype(handler)>(handler)](
                                              UoWPtr uow, CommandPtr cmd)
            { return h(uow, std::static_pointer_cast<T>(cmd)); };
        }

        std::vector<std::string> Handle(UoWFactory uowFactory, const CommandPtr& command);

    private:
        MessageBus();

        void HandleEvent(
            UoWPtr uow, EventPtr event, std::queue<Domain::IMessagePtr>& queue) noexcept;
        std::optional<std::string> HandleCommand(
            UoWPtr uow, CommandPtr command, std::queue<Domain::IMessagePtr>& queue);

        std::unordered_map<std::type_index, std::vector<EventHandler>> _eventHandlers;
        std::unordered_map<std::type_index, CommandHandler> _commandHandlers;

        MessageBus(const MessageBus&) = delete;
        MessageBus& operator=(const MessageBus&) = delete;
    };
}