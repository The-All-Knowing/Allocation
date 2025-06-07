#pragma once

#include "Precompile.h"
#include "AbstractUnitOfWork.h"
#include "Domain/Ports/IRepository.h"
#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services::UoW
{
    class SqlUnitOfWork final : public AbstractUnitOfWork 
    {
        struct Impl;

    public:
        SqlUnitOfWork();
        ~SqlUnitOfWork();

        Poco::Data::Session& GetSession() const noexcept;

        void Commit() override;
        void RollBack() override;

        Domain::IRepository& GetProductRepository() override;

    private:
        std::unique_ptr<Impl> _impl;
    };
}