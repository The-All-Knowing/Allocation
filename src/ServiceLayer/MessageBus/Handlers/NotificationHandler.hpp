#pragma once

#include "Precompile.hpp"

#include "Domain/Events/AbstractEvent.hpp"


namespace Allocation::ServiceLayer::Handlers
{
    /// Концепция для отправителей уведомлений.
    template <typename T>
    concept NotificationSender = requires(T t, std::string to, std::string msg) {
        { t(to, msg) } -> std::same_as<void>;
    };

    template <typename Message, NotificationSender Notifier>
        requires std::derived_from<Message, Domain::Events::AbstractEvent>
    class NotificationHandler
    {
    public:
        NotificationHandler(Notifier notifier = {}) : _notifier(std::move(notifier)) {}

        void operator()(std::shared_ptr<Message> event) const
        {
            _notifier("stock@made.com", std::format("Out of stock for {}", event->SKU));
        }

    private:
        Notifier _notifier;
    };
}