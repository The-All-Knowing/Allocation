#include "Services.h"
#include "Exceptions/InvalidSku.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Services
{
    void AddBatch(Domain::IUnitOfWork& uow,
        std::string_view ref, std::string_view SKU, size_t qty,
        std::optional<std::chrono::year_month_day> ETA)
    {
        auto& repo = uow.GetProductRepository();
        std::shared_ptr<Domain::Product> product = repo.Get(SKU);
        if (!product)
            product = std::make_shared<Domain::Product>(SKU);

        product->AddBatch(Domain::Batch(ref, SKU, qty, ETA));
        repo.Add(product);
        uow.Commit();
    }

    std::string Allocate(
        Domain::IUnitOfWork& uow,
        std::string_view orderid, std::string_view sku, size_t qty)
    {
        Domain::OrderLine line(std::string(orderid), std::string(sku), qty);

        auto& products = uow.GetProductRepository();
        auto product = products.Get(sku);
        if (!product)
            throw Exceptions::InvalidSku(sku);
        
        auto ref = product->Allocate(line);
        products.Add(product);
        uow.Commit();
        
        return ref;
    }
}