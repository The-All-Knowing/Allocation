#pragma once

#include "AbstractUnitOfWork.hpp"
#include "Domain/Ports/IRepository.hpp"
#include "Precompile.hpp"


namespace Allocation::Services::UoW
{
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
        struct Impl;

    public:
        SqlUnitOfWork();
        ~SqlUnitOfWork();

        [[nodiscard]] Poco::Data::Session& GetSession() const noexcept;

        void Commit() override;
        void RollBack() override;

        [[nodiscard]] Domain::IRepository& GetProductRepository() override;
        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override;

    private:
        std::unique_ptr<Impl> _impl;
    };

    [[nodiscard]] std::shared_ptr<Domain::IUnitOfWork> SqlUowFactory();
}