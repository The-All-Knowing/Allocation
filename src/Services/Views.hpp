#pragma once

#include "Precompile.hpp"
#include "Services/UoW/SqlUnitOfWork.hpp"


namespace Allocation::Services::Views
{
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, std::shared_ptr<Services::UoW::SqlUnitOfWork> uow);
}