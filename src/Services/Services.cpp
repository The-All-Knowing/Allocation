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

    void AddBatch(IRepositoryPtr repo,
        Poco::Data::Session& session,
        std::string ref, std::string sku, int qty,
        std::optional<std::chrono::year_month_day> ETA)
    {
        session.begin();
        Domain::Batch batch(ref, sku, qty, ETA);
        repo->Add(batch);
        session.commit();
    }

    std::string Allocate(
        IRepositoryPtr repo,
        Poco::Data::Session& session,
        std::string orderid, std::string sku, int qty)
    {
        Domain::OrderLine line(orderid, sku, qty);
        session.begin();
        auto batches = repo->List();
        if (!IsValidSku(line.GetSKU(), batches))
            throw InvalidSku(std::format("Invalid sku {}", line.GetSKU()));

        auto batch = Domain::Allocate(line, batches.begin(), batches.end());
        repo->Add(batch);
        session.commit();
        return batch.GetReference();
    }
}