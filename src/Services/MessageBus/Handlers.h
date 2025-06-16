#pragma once

#include "Domain/Ports/IUnitOfWork.h"
#include "Domain/Events/OutOfStock.h"
#include "Domain/Events/Allocated.h"
#include "Domain/Commands/Allocate.h"
#include "Domain/Commands/ChangeBatchQuantity.h"
#include "Domain/Commands/CreateBatch.h"


namespace Allocation::Services::Handlers
{
    void SendOutOfStockNotification(std::shared_ptr<Domain::IUnitOfWork>, std::shared_ptr<Domain::Events::OutOfStock> event);

    void PublishAllocatedEvent(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Events::Allocated> event);

    std::optional<std::string> AddBatch(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Commands::CreateBatch> message);

    std::optional<std::string> Allocate(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Commands::Allocate> message);

    std::optional<std::string> ChangeBatchQuantity(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Commands::ChangeBatchQuantity> message);
}