#include "SqlUnitOfWork.h"
#include "Adapters/Repository/SqlRepository.h"
#include "Adapters/Repository/TrackingRepository.h"
#include "Session/SessionPool.h"


namespace Allocation::Services::UoW
{
    struct SqlUnitOfWork::Impl
    {
        Poco::Data::Session session;
        Adapters::Repository::TrackingRepository tracking;
        Adapters::Repository::SqlRepository repository;

        Impl()
            : session(Adapters::Database::SessionPool::Instance().GetSession()),
              repository(session),
              tracking(repository)
        {
            session.setTransactionIsolation(Poco::Data::Session::TRANSACTION_REPEATABLE_READ);
            session.begin();
        }

        void commit()
        {
            for (auto& [sku, old, newVersion] : tracking.GetChangedVersions())
                repository.UpdateVersion(sku, old, newVersion);
            
            session.commit();
        }

        void rollback() { session.rollback(); }
        Domain::IRepository& getRepo() { return tracking; }
    };

    SqlUnitOfWork::SqlUnitOfWork() : _impl(std::make_unique<Impl>()) {}
    SqlUnitOfWork::~SqlUnitOfWork() = default;

    void SqlUnitOfWork::Commit()
    {
        _impl->commit();
        AbstractUnitOfWork::Commit();
    }

    Poco::Data::Session& SqlUnitOfWork::GetSession() const noexcept
    {
        return _impl->session;
    }

    void SqlUnitOfWork::RollBack()
    {
        _impl->rollback();
        AbstractUnitOfWork::RollBack();
    }

    Domain::IRepository& SqlUnitOfWork::GetProductRepository()
    {
        return _impl->getRepo();
    }

    std::vector<Domain::IMessagePtr> SqlUnitOfWork::GetNewMessages() noexcept
    {
        std::vector<Domain::IMessagePtr> result;

        for (const auto& product : _impl->tracking.GetSeen())
        {
            auto& messages = product->Messages();
            result.insert(result.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }

        return result;
    }

    std::shared_ptr<Domain::IUnitOfWork> SqlUowFactory()
    {
        return std::make_shared<SqlUnitOfWork>();
    }
}