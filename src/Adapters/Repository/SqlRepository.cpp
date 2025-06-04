#pragma once

#include "SqlRepository.h"
#include "Adapters/Database/Mappers/BatchMapper.h"


namespace Allocation::Adapters::Repository
{

    SqlRepository::SqlRepository(Poco::Data::Session& session): _session(session)
    {}

    void SqlRepository::Add(const Domain::Batch& batch)
    {
        Database::Mapper::BatchMapper mapper(_session);

        if (auto id = mapper.GetBatchId(batch.GetReference()); id.has_value())
            mapper.Update(id.value(), batch);
        else
            mapper.Insert(batch);
    }

    std::optional<Domain::Batch> SqlRepository::Get(const std::string& reference)
    {
        Database::Mapper::BatchMapper mapper(_session);

        return mapper.FindByReference(reference); 
    }

    std::vector<Domain::Batch> SqlRepository::List()
    {
        Database::Mapper::BatchMapper mapper(_session);

        return mapper.GetAll();
    }

}