#pragma once

#include "Precompile.h"


namespace Allocation::Domain::Events
{
    
    struct IEvent
    {
        virtual ~IEvent() = default;
        virtual std::string Name() const = 0;
    };

    using IEventPtr = std::shared_ptr<IEvent>;
}