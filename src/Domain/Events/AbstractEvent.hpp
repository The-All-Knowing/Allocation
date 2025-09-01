#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Абстрактное событие.
    struct AbstractEvent : public IMessage
    {
        /// @brief Деструктор абстрактного события.
        ~AbstractEvent() override = default;

        /// @brief Получить тип сообщения.
        /// @return Тип сообщения.
        [[nodiscard]] Type GetType() const override { return Type::Event; };
    };

    using EventPtr = std::shared_ptr<AbstractEvent>;
}