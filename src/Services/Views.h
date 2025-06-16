#pragma once

#include "Precompile.h"
#include "Services/UoW/SqlUnitOfWork.h"


namespace Allocation::Services::Views
{
    std::vector<std::pair<std::string, std::string>> Allocations(std::string orderid, std::shared_ptr<Services::UoW::SqlUnitOfWork> uow);
}