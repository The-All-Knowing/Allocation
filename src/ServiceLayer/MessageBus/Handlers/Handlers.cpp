#include "Handlers.hpp"

#include "Domain/Product/Product.hpp"
#include "ServiceLayer/Exceptions.hpp"


namespace Allocation::ServiceLayer::Handlers
{
    void AddBatch(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::CreateBatch> message)
    {
        auto product = uow.GetProductRepository().Get(message->sku);
        if (!product)
        {
            product = std::make_shared<Domain::Product>(message->sku);
            uow.GetProductRepository().Add(product);
        }
        product->AddBatch(
            Allocation::Domain::Batch(message->ref, message->sku, message->qty, message->eta));
        uow.Commit();
    }

    void Allocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::Allocate> command)
    {
        Domain::OrderLine line(command->orderid, command->sku, command->qty);
        auto product = uow.GetProductRepository().Get(command->sku);
        if (!product)
            throw Exceptions::InvalidSku(command->sku);

        auto ref = product->Allocate(line);
        uow.Commit();
    }

    void Reallocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Deallocated> event)
    {
        Allocate(uow,
            std::make_shared<Domain::Commands::Allocate>(event->orderid, event->SKU, event->qty));
    }

    void ChangeBatchQuantity(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::ChangeBatchQuantity> command)
    {
        auto product = uow.GetProductRepository().GetByBatchRef(command->ref);
        if (!product)
            throw std::invalid_argument("Invalid reference.");
        product->ChangeBatchQuantity(command->ref, command->qty);
        uow.Commit();
    }

    void AddAllocationToReadModel(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Allocated> event)
    {
        auto session = uow.GetSession();
        if (!session.has_value())
            return;

        (*session) << R"(
            INSERT INTO allocation.allocations_view (orderid, sku, batchref)
            VALUES ($1, $2, $3)
        )",
            Poco::Data::Keywords::use(event->orderid), Poco::Data::Keywords::use(event->SKU),
            Poco::Data::Keywords::use(event->batchref), Poco::Data::Keywords::now;
        uow.Commit();
    }

    void RemoveAllocationFromReadModel(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Deallocated> event)
    {
        auto session = uow.GetSession();
        if (!session.has_value())
            return;

        (*session) << R"(
            DELETE FROM allocation.allocations_view
            WHERE orderid = $1 AND sku = $2
        )",
            Poco::Data::Keywords::use(event->orderid), Poco::Data::Keywords::use(event->SKU);
        uow.Commit();
    }
}