#include "AbstractUnitOfWork.hpp"


namespace Allocation::Services::UoW
{
    void AbstractUnitOfWork::Commit() { _isCommited = true; }

    void AbstractUnitOfWork::RollBack() { _isCommited = false; }

    bool AbstractUnitOfWork::IsCommited() const noexcept { return _isCommited; }
}