#pragma once

#include "ILogger.hpp"


namespace Allocation::Services::Loggers
{

    struct PocoLogger : public ILogger
    {
        void Debug(const std::string& message) override
        {
            Poco::Util::Application::instance().logger().debug(message);
        };

        void Error(const std::string& message) override
        {
            Poco::Util::Application::instance().logger().error(message);
        };

        void Information(const std::string& message) override
        {
            Poco::Util::Application::instance().logger().information(message);
        };

        ~PocoLogger() override = default;
    };
}