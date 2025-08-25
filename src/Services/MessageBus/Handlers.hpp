#pragma once

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Commands/CreateBatch.hpp"
#include "Domain/Events/Allocated.hpp"
#include "Domain/Events/OutOfStock.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::Services::Handlers
{
    void SendOutOfStockNotification(
        std::shared_ptr<Domain::IUnitOfWork>, std::shared_ptr<Domain::Events::OutOfStock> event);

    void PublishAllocatedEvent(
        std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Events::Allocated> event);

    std::optional<std::string> AddBatch(std::shared_ptr<Domain::IUnitOfWork> uow,
        std::shared_ptr<Domain::Commands::CreateBatch> message);

    std::optional<std::string> Allocate(std::shared_ptr<Domain::IUnitOfWork> uow,
        std::shared_ptr<Domain::Commands::Allocate> message);

    std::optional<std::string> ChangeBatchQuantity(std::shared_ptr<Domain::IUnitOfWork> uow,
        std::shared_ptr<Domain::Commands::ChangeBatchQuantity> message);
}