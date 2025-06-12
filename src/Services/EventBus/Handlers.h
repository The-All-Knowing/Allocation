#pragma once

#include "Domain/Events/OutOfStock.h"


namespace Allocation::Services::Handlers
{
    void SendOutOfStockNotification(std::shared_ptr<Domain::Events::OutOfStock> event);
}