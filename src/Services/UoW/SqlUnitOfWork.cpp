#include "SqlUnitOfWork.hpp"


namespace Allocation::Services::UoW
{
    SqlUnitOfWork::SqlUnitOfWork() : AbstractUnitOfWork(_repository) {}

    void SqlUnitOfWork::Commit()
    {
        _session.commit();
        AbstractUnitOfWork::Commit();
    }

    std::optional<Poco::Data::Session> SqlUnitOfWork::GetSession() noexcept { return _session; }

    void SqlUnitOfWork::RollBack()
    {
        _session.rollback();
        AbstractUnitOfWork::RollBack();
    }
}