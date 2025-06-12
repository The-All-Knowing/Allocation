#include "Handlers.h"
#include "Adapters/Email/Email.h"


namespace Allocation::Services::Handlers
{
    void SendOutOfStockNotification(std::shared_ptr<Domain::Events::OutOfStock> event)
    {
        Adapters::Email::SendMail("stock@made.com", std::format("Out of stock for {}", event->SKU));
    }
}