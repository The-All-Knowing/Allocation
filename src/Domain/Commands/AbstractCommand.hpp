#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Абстрактная команда.
    struct AbstractCommand : public IMessage
    {
        /// @brief Деструктор абстрактной команды.
        ~AbstractCommand() override = default;

        /// @brief Возвращает тип сообщения.
        /// @return Тип сообщения.
        [[nodiscard]] Type GetType() const override { return Type::Command; };
    };

    using CommandPtr = std::shared_ptr<AbstractCommand>;
}