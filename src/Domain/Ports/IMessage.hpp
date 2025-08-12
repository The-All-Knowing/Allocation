#pragma once

#include "Precompile.hpp"


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
        [[nodiscard]] virtual std::string Name() const = 0;
        [[nodiscard]] virtual Type GetType() const = 0;
    };

    using IMessagePtr = std::shared_ptr<IMessage>;
}