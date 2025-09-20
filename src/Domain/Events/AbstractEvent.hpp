#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Абстрактное событие.
    struct AbstractEvent : public IMessage
    {
        /// @brief Деструктор.
        ~AbstractEvent() override = default;

        /// @brief Возвращает тип сообщения.
        /// @return Тип сообщения.
        [[nodiscard]] Type GetType() const override { return Type::Event; };
    };

    using EventPtr = std::shared_ptr<AbstractEvent>;
    using EventAttribute = std::pair<std::string, std::string>;

    /// @brief Читает аттрибуты события.
    /// @tparam Event Тип события.
    /// @param event Событие.
    /// @return Атрибуты события: название - значение.
    template <typename Event>
        requires std::derived_from<Event, Domain::Events::AbstractEvent>
    std::vector<EventAttribute> GetAttributes(EventPtr event)
    {
        static_assert(sizeof(Event) == 0, "GetAttributes not implemented for this Event type");
        return {};
    }
}