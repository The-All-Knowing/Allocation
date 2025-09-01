#include "Handlers.hpp"

#include "Adapters/Email/Email.hpp"
#include "Adapters/Redis/RedisEventPublisher.hpp"
#include "Domain/Product/Product.hpp"
#include "Infrastructure/Redis/RedisConfig.hpp"
#include "Infrastructure/Services/Exceptions/Errors.hpp"


namespace Allocation::Services::Handlers
{
    void AddBatch(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::CreateBatch> message)
    {
        auto product = uow.GetProductRepository().Get(message->sku);
        if (!product)
            product = std::make_shared<Domain::Product>(message->sku);

        product->AddBatch(
            Allocation::Domain::Batch(message->ref, message->sku, message->qty, message->eta));
        uow.GetProductRepository().Add(*product);
        uow.Commit();
    }

    void Allocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Commands::Allocate> command)
    {
        Domain::OrderLine line(command->orderid, command->sku, command->qty);
        auto product = uow.GetProductRepository().Get(command->sku);
        if (!product)
            throw Exceptions::InvalidSku(command->sku);

        auto ref = product->Allocate(line);
        uow.GetProductRepository().Add(*product);
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
        product->ChangeBatchQuantity(command->ref, command->qty);
        uow.GetProductRepository().Add(*product);
        uow.Commit();
    }

    void SendOutOfStockNotification(
        std::shared_ptr<Domain::IUnitOfWork>, std::shared_ptr<Domain::Events::OutOfStock> event)
    {
        Adapters::Email::SendMail("stock@made.com", std::format("Out of stock for {}", event->SKU));
    }

    void PublishAllocatedEvent(
        std::shared_ptr<Domain::IUnitOfWork>, std::shared_ptr<Domain::Events::Allocated> event)
    {
        auto config = Infrastructure::Redis::RedisConfig::FromConfig();
        Adapters::Redis::RedisEventPublisher publisher(config->host, config->port);
        publisher.Publish("line_allocated", event);
    }

    void AddAllocationToReadModel(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Allocated> event)
    {
        auto session = uow.GetSession().lock();
        if (!session)
            return;

        (*session) << R"(
            INSERT INTO allocations_view (orderid, sku, batchref)
            VALUES ($1, $2, $3)
        )",
            Poco::Data::Keywords::use(event->orderid), Poco::Data::Keywords::use(event->SKU),
            Poco::Data::Keywords::use(event->batchref), Poco::Data::Keywords::now;
        uow.Commit();
    }

    void RemoveAllocationFromReadModel(
        Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::Deallocated> event)
    {
        auto session = uow.GetSession().lock();
        if (!session)
            return;

        (*session) << R"(
            DELETE FROM allocations_view
            WHERE orderid = $1 AND sku = $2
        )",
            Poco::Data::Keywords::use(event->orderid), Poco::Data::Keywords::use(event->SKU);
        uow.Commit();
    }
}