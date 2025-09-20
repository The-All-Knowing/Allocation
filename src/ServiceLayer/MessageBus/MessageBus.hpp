#pragma once

#include "Domain/Commands/AbstractCommand.hpp"
#include "Domain/Events/AbstractEvent.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::ServiceLayer
{
    /// @brief Концепция для обработчиков событий конкретного типа.
    template <typename F, typename T>
    concept EventHandlerFor =
        std::derived_from<T, Domain::Events::AbstractEvent> &&
        (
            std::is_invocable_v<F, Domain::IUnitOfWork&, std::shared_ptr<T>> ||
            std::is_invocable_v<F, std::shared_ptr<T>>
        );

    /// @brief Концепция для обработчиков команд конкретного типа.
    template <typename F, typename T>
    concept CommandHandlerFor =
        std::derived_from<T, Domain::Commands::AbstractCommand> &&
        std::is_invocable_v<F, Domain::IUnitOfWork&, std::shared_ptr<T>>;

    /// @brief Шина сообщений для обработки событий и команд.
    class MessageBus
    {
        using EventHandler = std::function<void(Domain::IUnitOfWork&, Domain::Events::EventPtr)>;
        using CommandHandler =
            std::function<void(Domain::IUnitOfWork&, Domain::Commands::CommandPtr)>;

    public:
        /// @brief Возвращает экземпляр.
        /// @return Экземпляр шины сообщений.
        static MessageBus& Instance();

        /// @brief Подписывает обработчик на событие конкретного типа.
        /// @tparam T Тип события, производный от Domain::Events::AbstractEvent.
        /// @tparam F Тип обработчика.
        /// @param handler Обработчик события.
        template <typename T, typename F>
        requires EventHandlerFor<F, T>
        void SubscribeToEvent(F&& handler) noexcept
        {
            auto& handlers = _eventHandlers[typeid(T)];
            handlers.emplace_back(
                [h = std::forward<F>(handler)](
                    Domain::IUnitOfWork& uow, Domain::Events::EventPtr event)
                {
                    if constexpr (std::is_invocable_v<F, Domain::IUnitOfWork&, std::shared_ptr<T>>)
                        h(uow, std::static_pointer_cast<T>(event));
                    else if constexpr (std::is_invocable_v<F, std::shared_ptr<T>>)
                        h(std::static_pointer_cast<T>(event));
                });
        }

        /// @brief Устанавливает обработчик для команды конкретного типа.
        /// @tparam T Тип команды, производный от Domain::Commands::AbstractCommand.
        /// @tparam F Тип функции-обработчика.
        /// @param handler Обработчик команды.
        template <typename T, typename F>
        requires CommandHandlerFor<F, T>
        void SetCommandHandler(F&& handler) noexcept
        {
            _commandHandlers[typeid(T)] =
                [h = std::forward<F>(handler)](Domain::IUnitOfWork& uow,
                                            Domain::Commands::CommandPtr cmd)
                {
                    h(uow, std::static_pointer_cast<T>(cmd));
                };
        }

        /// @brief Обрабатывает входящее доменное сообщение.
        /// @param message Доменное сообщение.
        /// @param uow Единица работы для обработки сообщения.
        void Handle(Domain::IMessagePtr message, Domain::IUnitOfWork& uow);

        /// @brief Обрабатывает входящее доменное сообщение.
        /// @param message Доменное сообщение.
        /// @note Автоматически создаёт единицу работы SqlUnitOfWork.
        void Handle(Domain::IMessagePtr message);

        /// @brief Очищает все зарегистрированные обработчики событий и команд.
        void ClearHandlers() noexcept;

    private:
        /// @brief Конструктор.
        MessageBus() = default;

        /// @brief Удалённый конструктор копирования.
        MessageBus(const MessageBus&) = delete;

        /// @brief Удалённый оператор присваивания.
        MessageBus& operator=(const MessageBus&) = delete;

        /// @brief Удалённый оператор перемещения.
        MessageBus& operator=(MessageBus&&) = delete;

        /// @brief Обрабатывает входящее событие.
        /// @param uow Единица работы для обработки события.
        /// @param event Доменное событие.
        /// @param queue Очередь для новых сообщений.
        void HandleEvent(Domain::IUnitOfWork& uow, Domain::Events::EventPtr event,
            std::queue<Domain::IMessagePtr>& queue) noexcept;

        /// @brief Обрабатывает входящую команду.
        /// @param uow Единица работы для обработки команды.
        /// @param command Доменная команда.
        /// @param queue Очередь для новых сообщений.
        void HandleCommand(Domain::IUnitOfWork& uow, Domain::Commands::CommandPtr command,
            std::queue<Domain::IMessagePtr>& queue);

        std::unordered_map<std::type_index, std::vector<EventHandler>> _eventHandlers;
        std::unordered_map<std::type_index, CommandHandler> _commandHandlers;
    };
}