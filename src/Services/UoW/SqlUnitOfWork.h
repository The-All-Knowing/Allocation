#pragma once

#include "Precompile.h"
#include "AbstractUnitOfWork.h"
#include "Domain/Ports/IRepository.h"


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
        std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override;

    private:
        std::unique_ptr<Impl> _impl;
    };

    std::shared_ptr<Domain::IUnitOfWork> SqlUowFactory(); 
}