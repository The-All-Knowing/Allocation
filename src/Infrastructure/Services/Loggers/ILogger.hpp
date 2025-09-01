#pragma once

#include "Precompile.hpp"


namespace Allocation::Services::Loggers
{
    struct ILogger
    {
        virtual void Debug(const std::string& message) = 0;
        virtual void Error(const std::string& message) = 0;
        virtual void Information(const std::string& message) = 0;
        virtual ~ILogger() = default;
    };

    using ILoggerPtr = std::shared_ptr<Loggers::ILogger>;

    ILoggerPtr GetLogger();

    void InitializeLogger(ILoggerPtr logger);
}