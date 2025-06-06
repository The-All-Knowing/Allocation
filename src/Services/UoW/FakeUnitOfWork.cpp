#include "FakeUnitOfWork.h"


namespace Allocation::Services::UoW
{
    FakeUnitOfWork::FakeUnitOfWork(Adapters::Repository::FakeRepository& repo) : _repo(repo)
    {}

    Domain::IRepository& FakeUnitOfWork::GetBatchRepository()
    {
        return _repo;
    }
}