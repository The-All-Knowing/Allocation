#pragma once

#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::ServiceLayer::Loggers
{
    /// @brief Реализация логгера на основе Poco.
    struct PocoLogger : public Allocation::Loggers::ILogger
    {
        /// @brief Логирует сообщение уровня Debug.
        /// @param message Сообщение для логирования.
        void Debug(const std::string& message) override
        {
            Poco::Util::Application::instance().logger().debug(message);
        }

        /// @brief Логирует сообщение уровня Error.
        /// @param message Сообщение для логирования.
        void Error(const std::string& message) override
        {
            Poco::Util::Application::instance().logger().error(message);
        }

        /// @brief Логирует сообщение уровня Information.
        /// @param message Сообщение для логирования.
        void Information(const std::string& message) override
        {
            Poco::Util::Application::instance().logger().information(message);
        }
    };
}