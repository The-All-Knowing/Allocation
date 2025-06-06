#include "Services.h"
#include "Model.h"
#include "InvalidSku.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Services
{
    bool IsValidSku(std::string sku, const std::vector<Domain::Batch>& batches)
    {
        auto it = std::find_if(batches.begin(), batches.end(),
        [&sku](const auto& batch){return batch.GetSKU() == sku;});

        return it != batches.end();
    }

    void AddBatch(Domain::IUnitOfWork& uow,
        std::string ref, std::string sku, int qty,
        std::optional<std::chrono::year_month_day> ETA)
    {
        Domain::Batch batch(ref, sku, qty, ETA);
        uow.GetBatchRepository().Add(batch);
        uow.Commit();
    }

    std::string Allocate(
        Domain::IUnitOfWork& uow,
        std::string orderid, std::string sku, int qty)
    {
        Domain::OrderLine line(orderid, sku, qty);
        auto batches = uow.GetBatchRepository().List();
        if (!IsValidSku(line.GetSKU(), batches))
            throw InvalidSku(std::format("Invalid sku {}", line.GetSKU()));

        auto batch = Domain::Allocate(line, batches.begin(), batches.end());
        uow.GetBatchRepository().Add(batch);
        uow.Commit();
        return batch.GetReference();
    }
}