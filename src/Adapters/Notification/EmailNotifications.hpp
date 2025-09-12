#pragma once

#include "Precompile.hpp"

#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Notification
{
    /// @brief Простой отправитель email-уведомлений.
    class EmailSender
    {
    public:
        /// @brief Иммитирует отправку email-уведомления.
        /// @param to Адрес получателя.
        /// @param message Текст сообщения.
        void operator()(std::string to, std::string message) const
        {
            Allocation::Loggers::GetLogger()->Debug(
                std::format("Sending email to {}: {}", to, message));
        }
    };
}