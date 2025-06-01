#pragma once

#include "Precompile.h"
#include "Forwards.h"
#include "OrderLine.h"


namespace Allocation::Services
{
    std::string Allocate(
        const Domain::OrderLine& line,
        IRepositoryPtr repo,
        Poco::Data::Session& session);
}