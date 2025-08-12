#pragma once

#include "Precompile.hpp"


namespace Allocation::Adapters::Database
{
    void InitDatabase(Poco::Data::Session& session);
    void DropDatabase(Poco::Data::Session& session);
}