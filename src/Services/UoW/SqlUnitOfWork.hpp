#pragma once

#include "Precompile.hpp"

#include "AbstractUnitOfWork.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"


namespace Allocation::Services::UoW
{
    /// @brief SQL реализация единицы работы (Unit of Work).
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
    public:
        /// @brief Конструктор.
        SqlUnitOfWork();

        /// @brief Получение сессии базы данных.
        /// @return Сессию базы данных.
        [[nodiscard]] std::optional<Poco::Data::Session> GetSession() noexcept override;

        /// @brief Подтверждение изменений.
        void Commit() override;

        /// @brief Откат изменений.
        void RollBack() override;

    private:
        Poco::Data::Session _session{Adapters::Database::SessionPool::Instance().GetSession()};
        Adapters::Repository::SqlRepository _repository{_session};
    };
}