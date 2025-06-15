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
        virtual [[nodiscard]] std::string Name() const = 0;
        virtual [[nodiscard]] Type GetType() const = 0;
    };

    using IMessagePtr = std::shared_ptr<IMessage>;
}