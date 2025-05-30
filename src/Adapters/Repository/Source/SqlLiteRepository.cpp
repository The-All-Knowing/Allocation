#pragma once

#include "SqlLiteRepository.h"
#include "Adapters/Database/Include/Mappers/BatchMapper.h"


namespace Allocation::Adapters::Repository
{

    SqlLiteRepository::SqlLiteRepository(Poco::Data::Session& session): _session(session)
    {}

    
    void SqlLiteRepository::Add(const Domain::Batch& batch)
    {
        Database::Mapper::BatchMapper mapper(_session);

        if (auto id = mapper.GetBatchId(batch.GetReference()); id.has_value())
            mapper.Update(id.value(), batch);
        else
            mapper.Insert(batch);
    }

    std::optional<Domain::Batch> SqlLiteRepository::Get(const std::string& reference)
    {
        Database::Mapper::BatchMapper mapper(_session);

        return mapper.FindByReference(reference); 
    }

    std::vector<Domain::Batch> SqlLiteRepository::List()
    {
        Database::Mapper::BatchMapper mapper(_session);

        return mapper.GetAll();
    }

}