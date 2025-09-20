#pragma once

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

    /// @brief Возвращает экземпляр логгера.
    /// @return Указатель на логгер.
    /// @throw std::runtime_error Если логгер не инициализирован.
    ILoggerPtr GetLogger();

    /// @brief Инициализирует логгер.
    /// @param logger Инициализирующий логгер.
    void InitializeLogger(ILoggerPtr logger);

    /// @brief Проверяет инициализирован ли логгер.
    /// @return true - инициализирован, иначе false.
    bool IsInitialize();
}