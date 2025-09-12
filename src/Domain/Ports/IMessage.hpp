#pragma once

#include "Precompile.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс сообщения.
    struct IMessage
    {
        /// @brief Тип сообщения.
        enum class Type : int
        {
            Event,
            Command
        };

        /// @brief Деструктор интерфейса сообщения.
        virtual ~IMessage() = default;

        /// @brief Возвращает имя сообщения.
        /// @return Имя сообщения.
        [[nodiscard]] virtual std::string Name() const = 0;

        /// @brief Возвращает тип сообщения.
        /// @return Тип сообщения.
        [[nodiscard]] virtual Type GetType() const = 0;
    };

    using IMessagePtr = std::shared_ptr<IMessage>;
}