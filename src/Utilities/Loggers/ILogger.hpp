#pragma once

#include "Precompile.hpp"


namespace Allocation::Loggers
{
    /// @brief Интерфейс логгера для логирования сообщений разного уровня.
    struct ILogger
    {
        /// @brief Деструктор.
        virtual ~ILogger() = default;

        /// @brief Логирует сообщение уровня Debug.
        /// @param message Сообщение для логирования.
        virtual void Debug(const std::string& message) = 0;

        /// @brief Логирует сообщение уровня Error.
        /// @param message Сообщение для логирования.
        virtual void Error(const std::string& message) = 0;

        /// @brief Логирует сообщение уровня Information.
        /// @param message Сообщение для логирования.
        virtual void Information(const std::string& message) = 0;
    };

    using ILoggerPtr = std::shared_ptr<Loggers::ILogger>;

    /// @brief Получает экземпляр логгера.
    /// @return Указатель на логгер.
    ILoggerPtr GetLogger();

    /// @brief Инициализирует логгер.
    /// @param logger Указатель на логгер.
    void InitializeLogger(ILoggerPtr logger);
}