#include "SqlUnitOfWork.hpp"

#include "Adapters/Database/Session/SessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"
#include "Adapters/Repository/TrackingRepository.hpp"


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

    std::optional<Poco::Data::Session> SqlUnitOfWork::GetSession() noexcept
    {
        return _impl->session;
    }

    void SqlUnitOfWork::RollBack()
    {
        _impl->rollback();
        AbstractUnitOfWork::RollBack();
    }

    Domain::IRepository& SqlUnitOfWork::GetProductRepository() { return _impl->getRepo(); }

    std::vector<Domain::IMessagePtr> SqlUnitOfWork::GetNewMessages() noexcept
    {
        _newMessages.clear();
        for (const auto& product : _impl->tracking.GetSeen())
        {
            auto& messages = product->Messages();
            _newMessages.insert(messages.begin(), messages.end());
            product->ClearMessages();
        }

        return std::vector<Domain::IMessagePtr>(_newMessages.begin(), _newMessages.end());
    }

    std::shared_ptr<Domain::IUnitOfWork> SqlUowFactory()
    {
        return std::make_shared<SqlUnitOfWork>();
    }
}