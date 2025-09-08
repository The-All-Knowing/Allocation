#pragma once

#include "Precompile.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Конфигурация подключения к Redis.
    struct RedisConfig
    {
        std::string path;
        Poco::Int16 port;
    };
}