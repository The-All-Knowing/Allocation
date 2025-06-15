#include "Handlers.h"
#include "Adapters/Email/Email.h"
#include "Domain/Product/Product.h"
#include "Exceptions/InvalidSku.h"


namespace Allocation::Services::Handlers
{
    void SendOutOfStockNotification(std::shared_ptr<Domain::IUnitOfWork>, std::shared_ptr<Domain::Events::OutOfStock> event)
    {
        Adapters::Email::SendMail("stock@made.com", std::format("Out of stock for {}", event->SKU));
    }

    std::optional<std::string> AddBatch(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Commands::CreateBatch> message)
    {
        auto& repo = uow->GetProductRepository();
        std::shared_ptr<Domain::Product> product = repo.Get(message->sku);
        if (!product)
            product = std::make_shared<Domain::Product>(message->sku);

        product->AddBatch(Allocation::Domain::Batch(message->ref, message->sku, message->qty, message->eta));
        repo.Add(product);
        uow->Commit();

        return std::nullopt;
    }

    std::optional<std::string> Allocate(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Commands::Allocate> message)
    {
        Domain::OrderLine line(std::string(message->orderid), std::string(message->sku), message->qty);

        auto& products = uow->GetProductRepository();
        auto product = products.Get(message->sku);
        if (!product)
            throw Exceptions::InvalidSku(message->sku);
        
        auto ref = product->Allocate(line);
        products.Add(product);
        uow->Commit();
        
        return ref;
    }

    std::optional<std::string> ChangeBatchQuantity(std::shared_ptr<Domain::IUnitOfWork> uow, std::shared_ptr<Domain::Commands::ChangeBatchQuantity> message)
    {
        auto product = uow->GetProductRepository().GetByBatchRef(message->ref);
        product->ChangeBatchQuantity(message->ref, message->qty);
        uow->Commit();

        return std::nullopt;
    }
}