#pragma once

#include "Precompile.hpp"


namespace Allocation::Adapters::Database
{
    struct ConnectionConfig
    {
        std::string connector;
        std::string connectionString;
        int minSessions{1};
        int maxSessions{32};
        int idleTime{60};
        int connTimeout{60};
    };
}