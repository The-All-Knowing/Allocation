#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain::Commands
{
    struct AbstractCommand : public IMessage
    {
        ~AbstractCommand() override = default;
        [[nodiscard]] Type GetType() const override { return Type::Command; };
    };
}