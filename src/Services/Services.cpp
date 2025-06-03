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

    std::string Allocate(
        const Domain::OrderLine& line,
        IRepositoryPtr repo,
        Poco::Data::Session& session)
    {
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