#pragma once

namespace Allocation::Domain
{
    /// @brief Интерфейс сообщения.
    struct IMessage
    {
        /// @brief Типы сообщений.
        enum class Type : int
        {
            /// @brief Событие.
            Event,
            /// @brief Команда.
            Command
        };

        /// @brief Деструктор.
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