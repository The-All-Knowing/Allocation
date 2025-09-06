#pragma once

#include "Precompile.hpp"

#include "Adapters/Database/Session/ConnectionConfig.hpp"


namespace Allocation
{
    Adapters::Database::ConnectionConfig ReadSystemDatabaseConfigs();
}