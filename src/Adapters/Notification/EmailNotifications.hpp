#pragma once

#include "Precompile.hpp"

#include "Services/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Notification
{
    class EmailSender
    {
    public:
        void operator()(std::string to, std::string message) const
        {
            Services::Loggers::GetLogger()->Debug(
                std::format("Sending email to {}: {}", to, message));
        }
    };
}