#include "SqlUnitOfWork.h"
#include "Adapters/Repository/SqlRepository.h"
#include "Session/SessionPool.h"


namespace Allocation::Services::UoW
{
    struct SqlUnitOfWork::Impl
    {
        Poco::Data::Session session;
        Poco::Data::Transaction transaction;
        std::unique_ptr<Domain::IRepository> repository;

        Impl()
            : session(Adapters::Database::SessionPool::Instance().GetSession()),
              transaction(session, true),
              repository(std::make_unique<Adapters::Repository::SqlRepository>(session))
        {
        }

        void commit() { transaction.commit(); }
        void rollback() { transaction.rollback(); }
        Domain::IRepository& getRepo() { return *repository; }
    };

    SqlUnitOfWork::SqlUnitOfWork() : _impl(std::make_unique<Impl>()) {}
    SqlUnitOfWork::~SqlUnitOfWork() = default;

    void SqlUnitOfWork::Commit()
    {
        _impl->commit();
        AbstractUnitOfWork::Commit();
    }

    void SqlUnitOfWork::RollBack()
    {
        _impl->rollback();
        AbstractUnitOfWork::RollBack();
    }

    Domain::IRepository& SqlUnitOfWork::GetBatchRepository()
    {
        return _impl->getRepo();
    }
}