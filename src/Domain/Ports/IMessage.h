#pragma once

#include "Precompile.h"


namespace Allocation::Domain
{
    
    struct IMessage
    {
        enum class Type : int
        {
            Event,
            Command
        };

        virtual ~IMessage() = default;
        virtual std::string Name() const = 0;
        virtual Type GetType() const = 0;
    };

    using IMessagePtr = std::shared_ptr<IMessage>;
}