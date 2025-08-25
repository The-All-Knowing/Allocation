#pragma once

#include "Precompile.hpp"


namespace Allocation::Infrastructure::Redis
{
    struct RedisConfig
    {
        std::string host;
        int port;

        static std::shared_ptr<RedisConfig> FromConfig();
    };
}