#pragma once

#include "Domain/Events/AbstractEvent.hpp"


namespace Allocation::ServiceLayer::Handlers
{
    /// @brief Концепция для отправителей уведомлений.
    template <typename T>
    concept NotificationSender = requires(T t, const std::string& to, const std::string& msg) {
        { t(to, msg) } -> std::same_as<void>;
    };

    /// @brief Отправитель уведомлений.
    /// @tparam Message Тип сообщения.
    /// @tparam Notifier Тип отправителя уведомлений.
    template <typename Message, NotificationSender Notifier>
        requires std::derived_from<Message, Domain::Events::AbstractEvent>
    class NotificationHandler
    {
    public:
        /// @brief Конструктор.
        /// @param notifier Отправитель уведомлений.
        NotificationHandler(Notifier notifier = {}) : _notifier(std::move(notifier)) {}

        /// @brief Отправляет уведомление.
        /// @param event Событие "Нет в наличии товара".
        /// @details В данном примере адрес получателя и текст сообщения захардкожены.
        void operator()(std::shared_ptr<Message> event) const
        {
            _notifier("stock@made.com", std::format("Out of stock for {}", event->sku));
        }

    private:
        Notifier _notifier;
    };
}