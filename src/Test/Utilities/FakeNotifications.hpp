#pragma once

#include "Precompile.hpp"


namespace Allocation::Tests
{
    struct FakeNotifications
    {
        void operator()(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::OutOfStock> event)
        {
            
        }

        std::vector<std::string> sent;
    };
}