#pragma once

#include "Precompile.hpp"


namespace Allocation::Adapters::Notification
{
    class EmailSender
    {
    public:
        void operator()(std::string to, std::string message) const
        {
            std::cout << std::format("Sending email to {}: {}", to, message) << std::endl;
        }
    };
}