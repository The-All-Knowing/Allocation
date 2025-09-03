#pragma once

#include "Precompile.hpp"


namespace Allocation::Infrastructure::Redis::Handlers
{
    void HandleChangeBatchQuantity(const void* sender, Poco::Redis::RedisEventArgs& args);
}