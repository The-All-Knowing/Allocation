#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain::Commands
{
    /// @brief Абстрактная команда.
    struct AbstractCommand : public IMessage
    {
        /// @brief Деструктор.
        ~AbstractCommand() override = default;

        /// @brief Возвращает тип сообщения.
        /// @return Тип команды.
        [[nodiscard]] Type GetType() const override { return Type::Command; };
    };

    using CommandPtr = std::shared_ptr<AbstractCommand>;
}