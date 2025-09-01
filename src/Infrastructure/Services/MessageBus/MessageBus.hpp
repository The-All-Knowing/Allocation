#pragma once

#include "Precompile.hpp"

#include "Domain/Commands/AbstractCommand.hpp"
#include "Domain/Events/AbstractEvent.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::Services
{
    /// @brief Шина сообщений для обработки событий и команд.
    class MessageBus
    {
        using EventHandler = std::function<void(Domain::IUnitOfWork&, Domain::Events::EventPtr)>;
        using CommandHandler =
            std::function<void(Domain::IUnitOfWork&, Domain::Commands::CommandPtr)>;

    public:
        /// @brief Получает экземпляр MessageBus.
        /// @return Ссылка на единственный экземпляр MessageBus.
        static MessageBus& Instance();

        /// @brief Подписывает обработчик на событие конкретного типа.
        /// @tparam T Тип события, производный от Domain::Events::AbstractEvent.
        /// @param handler Функция-обработчик, принимающая два аргумента:
        ///        - ссылку на Domain::IUnitOfWork для взаимодействия с хранилищем
        ///        - умный указатель на событие типа T
        template <typename T>
            requires std::derived_from<T, Domain::Events::AbstractEvent>
        void SubscribeToEvent(auto&& handler) noexcept
        {
            auto& handlers = _eventHandlers[typeid(T)];
            handlers.emplace_back([h = std::forward<decltype(handler)>(handler)](
                                      Domain::IUnitOfWork& uow, Domain::Events::EventPtr event)
                { h(uow, std::static_pointer_cast<T>(event)); });
        }

        /// @brief Устанавливает обработчик для команды конкретного типа.
        /// @tparam T Тип команды, производный от Domain::Commands::AbstractCommand.
        /// @param handler Функция-обработчик, принимающая два аргумента:
        ///        - ссылку на Domain::IUnitOfWork для взаимодействия с хранилищем
        ///        - умный указатель на команду типа T
        template <typename T>
            requires std::derived_from<T, Domain::Commands::AbstractCommand>
        void SetCommandHandler(auto&& handler) noexcept
        {
            _commandHandlers[typeid(T)] =
                [h = std::forward<decltype(handler)>(handler)](Domain::IUnitOfWork& uow,
                    Domain::Commands::CommandPtr cmd) { h(uow, std::static_pointer_cast<T>(cmd)); };
        }

        /// @brief Обрабатывает входящее сообщение.
        /// @param message Умный указатель на сообщение.
        /// @param uow Единица работы для обработки сообщения.
        void Handle(Domain::IMessagePtr message, Domain::IUnitOfWork& uow);

        /// @brief Обрабатывает входящее сообщение.
        /// @param message Умный указатель на сообщение.
        void Handle(Domain::IMessagePtr message);

    private:
        /// @brief Конструктор по умолчанию.
        MessageBus();

        /// @brief Конструктор копирования.
        /// @param other Экземпляр MessageBus, который нужно скопировать.
        MessageBus(const MessageBus&) = delete;

        /// @brief Оператор присваивания.
        /// @param other Экземпляр MessageBus, который нужно скопировать.
        /// @return Ссылка на текущий объект.
        MessageBus& operator=(const MessageBus&) = delete;

        /// @brief Обрабатывает входящее событие.
        /// @param uow Единица работы для обработки события.
        /// @param event Умный указатель на событие.
        /// @param queue Очередь для новых сообщений.
        void HandleEvent(Domain::IUnitOfWork& uow, Domain::Events::EventPtr event,
            std::queue<Domain::IMessagePtr>& queue) noexcept;

        /// @brief Обрабатывает входящую команду.
        /// @param uow Единица работы для обработки команды.
        /// @param command Умный указатель на команду.
        /// @param queue Очередь для новых сообщений.
        void HandleCommand(Domain::IUnitOfWork& uow, Domain::Commands::CommandPtr command,
            std::queue<Domain::IMessagePtr>& queue);

        std::unordered_map<std::type_index, std::vector<EventHandler>> _eventHandlers;
        std::unordered_map<std::type_index, CommandHandler> _commandHandlers;
    };
}