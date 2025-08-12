#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain::Events
{
    struct AbstractEvent : public IMessage
    {
        ~AbstractEvent() override = default;
        [[nodiscard]] Type GetType() const override { return Type::Event; };
    };
}