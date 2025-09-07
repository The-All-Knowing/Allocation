#pragma once

#include "Precompile.hpp"

#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Notification
{
    class EmailSender
    {
    public:
        void operator()(std::string to, std::string message) const
        {
            Allocation::Loggers::GetLogger()->Debug(
                std::format("Sending email to {}: {}", to, message));
        }
    };
}