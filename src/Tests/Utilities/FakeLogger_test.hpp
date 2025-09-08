#pragma once

#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Tests
{
    /// @brief Фейковый реализация логгера для тестирования.
    struct FakeLogger : public Allocation::Loggers::ILogger
    {
        /// @brief Логирует сообщение уровня Debug.
        /// @param message Сообщение для логирования.
        void Debug(const std::string& message) override { std::cout << message << std::endl; };

        /// @brief Логирует сообщение уровня Error.
        /// @param message Сообщение для логирования.
        void Error(const std::string& message) override { std::cerr << message << std::endl; };

        /// @brief Логирует сообщение уровня Information.
        /// @param message Сообщение для логирования.
        void Information(const std::string& message) override { std::cerr << message << std::endl; }
    };
}