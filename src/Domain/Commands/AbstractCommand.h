#pragma once

#include "Ports/IMessage.h"


namespace Allocation::Domain::Commands
{
    
    struct AbstractCommand : public IMessage
    {
        ~AbstractCommand() override = default;
        Type GetType() const override {return Type::Command;};
    };
}