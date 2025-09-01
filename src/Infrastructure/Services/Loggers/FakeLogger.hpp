#pragma once

#include "ILogger.hpp"


namespace Allocation::Services::Loggers
{

    struct FakeLogger : public ILogger
    {
        void Debug(const std::string& message) override { std::cout << message << std::endl; };

        void Error(const std::string& message) override { std::cerr << message << std::endl; };

        void Information(const std::string& message) override { std::cerr << message << std::endl; }

        ~FakeLogger() override = default;
    };
}